import os
import numpy as np
import tensorflow as tf

model = None

# Define the image size
IMG_SIZE = (200, 200)


def load_model(model_path):
    # use h5 model
    global model

    model = tf.keras.models.load_model(model_path)

def predict_image(image_path, all_classes):
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

def train_model(class_names, save_path, initial_epochs=20, finetune_epochs=20):
    PATH = os.path.join(os.path.dirname("static/images/"))
    print(PATH)

    BATCH_SIZE = 2
    IMG_SIZE = (200, 200)

    train_dataset = tf.keras.utils.image_dataset_from_directory(PATH,
                                                                shuffle=True,
                                                                batch_size=BATCH_SIZE,
                                                                image_size=IMG_SIZE,
                                                                validation_split=0.2,
                                                                seed=123,
                                                                subset='training')

    validation_dataset = tf.keras.utils.image_dataset_from_directory(PATH,
                                                                     shuffle=True,
                                                                     batch_size=BATCH_SIZE,
                                                                     image_size=IMG_SIZE,
                                                                     validation_split=0.2,
                                                                     seed=123,
                                                                     subset='validation')

    class_names = train_dataset.class_names
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
    history = model.fit(train_dataset,
                        epochs=initial_epochs,
                        validation_data=validation_dataset)

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
              validation_data=validation_dataset)

    model.save(save_path)