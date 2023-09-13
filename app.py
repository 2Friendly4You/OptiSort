import os
import shutil
import zipfile
import json
from flask import Flask, render_template, request, jsonify, send_file
import matplotlib.pyplot as plt
import numpy as np
import tensorflow as tf
from time import sleep
import threading

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'static/images'
app.config['MODEL_FOLDER'] = 'static/models'
app.config['TRAINING_IN_PROGRESS'] = False  # Flag to track training status

# Initialize lists to hold classes
all_classes = []
selected_classes = []
rejected_classes = []

# Create a lock to control access to the training process
training_lock = threading.Lock()


def delete_and_create_folders():
    images_folder = app.config['UPLOAD_FOLDER']
    shutil.rmtree(images_folder)  # Delete the entire images folder
    os.makedirs(images_folder)    # Recreate the images folder

    model_folder = app.config['MODEL_FOLDER']
    shutil.rmtree(model_folder)  # Delete the entire images folder
    os.makedirs(model_folder)    # Recreate the images folder


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/trainmodel')
def trainmodel():
    return render_template('trainmodel.html')


@app.route('/usemodel')
def usemodel():
    return render_template('usemodel.html')


@app.route('/upload', methods=['POST'])
def upload():
    global all_classes

    # Handle the uploaded zip file
    zip_file = request.files['zip_file']
    if zip_file:
        # Save the zip file to the file system
        zip_file.save(os.path.join("uploads", zip_file.filename))

        # Unpack the zip file
        with zipfile.ZipFile(os.path.join("uploads", zip_file.filename), 'r') as zip_ref:
            zip_ref.extractall("uploads")

        # Load the all classes JSON
        with open(os.path.join("uploads", "config.json"), 'r') as json_file:
            json_data = json.load(json_file)
            all_classes = json_data["class_names"]

        # Simulate loading the model and sending scanned classes back
        # In reality, you'd load the model and extract classes here
        # Replace with actual scanned classes
        
        scanned_classes = all_classes
        return jsonify({"message": "Zip file uploaded and classes extracted successfully.", "scanned_classes": scanned_classes})

    return jsonify({"message": "No zip file uploaded."})


@app.route('/sort', methods=['POST'])
def sort():
    global selected_classes, rejected_classes
    selected_classes = request.form.getlist('selected_classes[]')
    rejected_classes = request.form.getlist('rejected_classes[]')

    print(selected_classes)
    print(rejected_classes)

    return jsonify({"message": "Sorting information saved successfully."})


@app.route('/train', methods=['POST'])
def train_image_classifier():
    # Check if training is already in progress
    if app.config['TRAINING_IN_PROGRESS']:
        return jsonify({"message": "Training is already in progress."}), 400

    # Set the training flag to indicate that training is in progress
    app.config['TRAINING_IN_PROGRESS'] = True

    try:
        class_count = int(request.form['class_count'])
        class_names = [
            request.form[f'class_name_{i}'] for i in range(class_count)]

        # Delete existing folders and create new ones
        delete_and_create_folders()

        # Process and save uploaded images for each class in separate folders
        for i, class_name in enumerate(class_names):
            class_folder = os.path.join(
                app.config['UPLOAD_FOLDER'], class_name)
            # Create class-specific folder if it doesn't exist
            os.makedirs(class_folder, exist_ok=True)

            class_input_name = f'class_{i}'
            uploaded_files = request.files.getlist(class_input_name)

            for file in uploaded_files:
                if file.filename != '':
                    # Get the file extension
                    file_extension = os.path.splitext(file.filename)[1]

                    # Create a unique filename for each image using a counter
                    image_counter = len(os.listdir(class_folder)) + 1
                    image_filename = f"image_{image_counter}{file_extension}"

                    # Save the image to the class-specific folder with the unique filename
                    image_path = os.path.join(class_folder, image_filename)
                    file.save(image_path)

        # sleep 1 second
        sleep(1)

        # Now you can train your TensorFlow model using the organized data
        train_model(class_names)

        # Create a dictionary to store class data
        class_data = {'class_names': class_names}

        # Create a config JSON file with class data
        config_file_path = os.path.join(
            app.config['MODEL_FOLDER'], 'config.json')
        with open(config_file_path, 'w') as config_file:
            json.dump(class_data, config_file)

        # Create a zip file containing the trained model, class names, and config file
        model_zip_filename = 'trained_model.zip'
        model_folder = app.config['MODEL_FOLDER']
        model_tflite = "static/models/model.tflite"
        with zipfile.ZipFile(os.path.join(model_folder, model_zip_filename), 'w') as zipf:
            # Add config file to the zip
            zipf.write(config_file_path, 'config.json')
            zipf.write(model_tflite, 'model.tflite')  # Add model to the zip

        # Send a JSON response with the link to download the trained model
        response_data = {
            "message": "Model trained successfully",
            "download_url": f"/download/{model_zip_filename}"
        }
        return jsonify(response_data)

    finally:
        # Reset the training flag when training is completed or an error occurs
        app.config['TRAINING_IN_PROGRESS'] = False


@app.route('/download/<filename>', methods=['GET'])
def download_file(filename):
    model_folder = app.config['MODEL_FOLDER']
    return send_file(os.path.join(model_folder, filename), as_attachment=True)


def train_model(class_names, epochs=40):
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
        tf.keras.layers.RandomFlip('horizontal_and_vertical'),
        tf.keras.layers.RandomRotation(0.2),
        tf.keras.layers.RandomZoom(0.2)
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

    initial_epochs = int(epochs/2)
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

    fine_tune_epochs = int(epochs/2)
    total_epochs = initial_epochs + fine_tune_epochs

    model.fit(train_dataset,
              epochs=total_epochs,
              initial_epoch=history.epoch[-1],
              validation_data=validation_dataset)

    model.save('static/models/model')

    # Convert the model
    converter = tf.lite.TFLiteConverter.from_saved_model(
        'static/models/model')  # path to the SavedModel directory
    tflite_model = converter.convert()

    # Save the model.
    with open('static/models/model.tflite', 'wb') as f:
        f.write(tflite_model)


if __name__ == '__main__':
    # Create the "uploads" directory if it doesn't exist
    os.makedirs("uploads", exist_ok=True)
    app.run(debug=False)
