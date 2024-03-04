import os
import numpy as np
import tensorflow as tf
from tensorflow.keras.callbacks import Callback
from flask_socketio import SocketIO

model = None

# Define the image size
IMG_SIZE = (200, 200)

sio = None  # SocketIO object

class ProgressCallback(Callback):
    def __init__(self, socketio, total_epochs):
        super().__init__()
        self.socketio = socketio
        self.total_epochs = total_epochs

    def on_epoch_end(self, epoch, logs=None):
        if logs is None:
            logs = {}
        percent_complete = (epoch + 1) / self.total_epochs * 100
        # Prepare the message with training status, including loss and accuracy metrics
        message = {
            'percent_complete': percent_complete,
            'status': 'training',
            'loss': logs.get('loss', 0),
            'accuracy': logs.get('accuracy', 0),
            'validation_loss': logs.get('val_loss', 0),
            'validation_accuracy': logs.get('val_accuracy', 0),
            'epoch': epoch + 1,
        }
        # Emit the progress update to all connected clients
        self.socketio.emit('training_progress', message)


def load_model(model_path):
    # use h5 model
    global model

    model = tf.keras.models.load_model(model_path)

def unload_model():
    global model
    model = None

def predict_image(image_path, all_classes):
    if model is None:
        raise ValueError("Model is not loaded")

    # Load the image you want to predict on
    img = tf.keras.preprocessing.image.load_img(
        image_path, target_size=IMG_SIZE)

    # Convert the image to a numpy array
    img_array = tf.keras.preprocessing.image.img_to_array(img)

    # Preprocess the image for MobileNetV2
    img_array = tf.keras.applications.mobilenet_v2.preprocess_input(img_array)

    # Add an extra dimension to the array to make it suitable for the model
    img_array = np.expand_dims(img_array, axis=0)

    # Use the loaded model to make a prediction
    predictions = model.predict(img_array)

    # The model's output is directly the class probabilities
    class_probabilities = predictions[0]

    # Find the class with the highest probability
    max_probability_index = np.argmax(class_probabilities)

    # Map the class name to its probability
    class_name = all_classes[max_probability_index]
    class_probability = class_probabilities[max_probability_index] * 100

    return class_name, class_probability

def train_model(class_names, save_path, initial_epochs=20, finetune_epochs=20, duplication_factor=4, validation_split=0.2, socketio=None):
    PATH = os.path.join(os.path.dirname("static/images/"))
    print(PATH)

    BATCH_SIZE = 2
    IMG_SIZE = (200, 200)

    train_dataset = tf.keras.utils.image_dataset_from_directory(PATH,
                                                                shuffle=True,
                                                                batch_size=BATCH_SIZE,
                                                                image_size=IMG_SIZE,
                                                                validation_split=validation_split,
                                                                seed=123,
                                                                subset='training')
    
    # Get the class names
    class_names = train_dataset.class_names

    validation_dataset = tf.keras.utils.image_dataset_from_directory(PATH,
                                                                     shuffle=True,
                                                                     batch_size=BATCH_SIZE,
                                                                     image_size=IMG_SIZE,
                                                                     validation_split=validation_split,
                                                                     seed=123,
                                                                     subset='validation')
    
    def duplicate_and_augment_images(image, label):
        images, labels = [], []

        for _ in range(duplication_factor):
            # Corrected to use the original image for each augmentation
            augmented_image = tf.image.random_brightness(image, max_delta=0.4)
            images.append(augmented_image)
            labels.append(label)
        
        images = tf.stack(images, axis=0)
        labels = tf.stack(labels, axis=0)

        return images, labels
    
    train_dataset = train_dataset.unbatch().map(duplicate_and_augment_images).unbatch().shuffle(1000).batch(BATCH_SIZE)
    validation_dataset = validation_dataset.unbatch().map(duplicate_and_augment_images).unbatch().shuffle(1000).batch(BATCH_SIZE)
    
    print(class_names)
    print('Number of validation batches: %d' %
          tf.data.experimental.cardinality(validation_dataset))

    AUTOTUNE = tf.data.AUTOTUNE

    train_dataset = train_dataset.prefetch(buffer_size=AUTOTUNE)
    validation_dataset = validation_dataset.prefetch(buffer_size=AUTOTUNE)

    data_augmentation = tf.keras.Sequential([
        tf.keras.layers.RandomZoom(0.1),
        tf.keras.layers.RandomBrightness(0.2),
    ])

    preprocess_input = tf.keras.applications.mobilenet_v2.preprocess_input
    rescale = tf.keras.layers.Rescaling(1./127.5, offset=-1)

    # Create the base model from the pre-trained model MobileNet V2
    IMG_SHAPE = IMG_SIZE + (3,)
    base_model = tf.keras.applications.MobileNetV2(input_shape=IMG_SHAPE,
                                                   include_top=False,
                                                   weights='imagenet')

    image_batch, label_batch = next(iter(train_dataset))
    feature_batch = base_model(image_batch)

    base_model.trainable = False

    global_average_layer = tf.keras.layers.GlobalAveragePooling2D()
    feature_batch_average = global_average_layer(feature_batch)
    print(feature_batch_average.shape)

    prediction_layer = tf.keras.layers.Dense(
        len(class_names), activation=tf.nn.softmax)
    prediction_batch = prediction_layer(feature_batch_average)
    print(prediction_batch.shape)

    inputs = tf.keras.Input(shape=(200, 200, 3))
    x = data_augmentation(inputs)
    x = preprocess_input(x)
    x = base_model(x, training=False)
    x = global_average_layer(x)
    x = tf.keras.layers.Dropout(0.2)(x)
    outputs = prediction_layer(x)
    model = tf.keras.Model(inputs, outputs)

    base_learning_rate = 0.0001
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=base_learning_rate),
                  loss=tf.keras.losses.SparseCategoricalCrossentropy(
                      from_logits=True),
                  metrics=['accuracy'])

    initial_epochs = int(initial_epochs)

    total_epochs = initial_epochs + finetune_epochs
    
    progress_callback = ProgressCallback(socketio, total_epochs=total_epochs)
    history = model.fit(train_dataset,
                        epochs=initial_epochs,
                        validation_data=validation_dataset,
                        callbacks=[progress_callback])

    base_model.trainable = True

    # Fine-tune from this layer onwards
    fine_tune_at = 100

    # Freeze all the layers before the `fine_tune_at` layer
    for layer in base_model.layers[:fine_tune_at]:
        layer.trainable = False

    model.compile(loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                  optimizer=tf.keras.optimizers.RMSprop(
                      learning_rate=base_learning_rate/10),
                  metrics=['accuracy'])

    fine_tune_epochs = int(finetune_epochs)
    total_epochs = initial_epochs + fine_tune_epochs

    model.fit(train_dataset,
              epochs=total_epochs,
              initial_epoch=history.epoch[-1],
              validation_data=validation_dataset,
              callbacks=[progress_callback])

    model.save(save_path)