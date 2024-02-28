import os
import sys
import shutil
import zipfile
import json
from flask import Flask, render_template, request, jsonify, send_file
from flask_socketio import SocketIO, emit, send
import matplotlib.pyplot as plt
import numpy as np
import tensorflow as tf
from time import sleep
import threading
import eventlet
import random
from threading import Thread
import serial
import cv2
from multiprocessing import Pool, freeze_support
import time
import copy
import camera
import tensorflow as tf
import model_functions as mf

app = Flask(__name__)

# config of program
UPLOAD_FOLDER = 'static/images'
MODEL_FOLDER = 'static/models'
TRAINING_IN_PROGRESS = False  # Flag to track training status

IMG_SIZE = (200, 200)
NUM_CAMERAS = 0
OFFSET = 0
PORT = "COM3"

# global vars
model = None

camera_indices = None
capture_objects = None

camera_manager = None

sorting_type = "dominant-reject"

socketio = SocketIO(app, async_mode="threading")

# Initialize lists to hold classes
all_classes = ['bad', 'good']
selected_classes = ['good']
rejected_classes = ['bad']

# Create a lock to control access to the training process
training_lock = threading.Lock()


def delete_and_create_folders():
    shutil.rmtree(UPLOAD_FOLDER)  # Delete the entire images folder
    os.makedirs(UPLOAD_FOLDER)    # Recreate the images folder

    shutil.rmtree(MODEL_FOLDER)  # Delete the entire images folder
    os.makedirs(MODEL_FOLDER)    # Recreate the images folder


def update_websocket_images(images):
    # Create a shallow copy of the images array
    copied_images = copy.copy(images)

    # Modify the copied array
    for i in range(len(copied_images)):
        copied_images[i] = copied_images[i] + "?" + str(int(time.time()))

    print(copied_images)
    socketio.emit('update_images', {'images': copied_images})


def update_websocket_text(text):
    socketio.emit('update_text', {'text': text})


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/trainmodel')
def trainmodel():
    return render_template('trainmodel.html')


@app.route('/usemodel')
def usemodel():
    return render_template('usemodel.html')

@app.route('/documentation')
def documentation():
    return render_template('documentation.html')

@app.route('/current-settings-and-models')
def current_settings_and_model():
    # currently loaded models are in the models folder
    return jsonify({"selected_classes": selected_classes, "rejected_classes": rejected_classes, "sorting_type": sorting_type, "model": model}), 200

@app.route('/upload', methods=['POST'])
def upload():
    global all_classes

    # Handle the uploaded zip file
    zip_file = request.files['zip_file']

    # Get the name of the zip file without the extension
    zip_file_name = os.path.splitext(zip_file.filename)[0]

    if zip_file:
        # create new folder with the name of the zip file, if it doesn't exist. if it exists, return error
        if not os.path.exists(os.path.join("models", zip_file_name)):
            os.makedirs(os.path.join("models", zip_file_name))
        else:
            return jsonify({"message": "A zip file with the same name already exists. Delete it from the main page."}), 400

        # Save the zip file to this folder
        zip_file.save(os.path.join("models", zip_file_name, zip_file.filename))

        # Unpack the zip file
        with zipfile.ZipFile(os.path.join("models", zip_file_name, zip_file.filename), 'r') as zip_ref:
            zip_ref.extractall(path=os.path.join("models", zip_file_name))

        # Load the all classes JSON
        with open(os.path.join("models", zip_file_name, "config.json"), 'r') as json_file:
            json_data = json.load(json_file)
            all_classes = json_data["class_names"]

        # Load the model
        mf.load_model(os.path.join("models", zip_file_name, "model.h5"))

        scanned_classes = all_classes
        return jsonify({"message": "Zip file uploaded and classes extracted successfully.", "scanned_classes": scanned_classes}), 200

    return jsonify({"message": "No zip file uploaded."}), 400


@app.route('/sort', methods=['POST'])
def sort():
    global selected_classes, rejected_classes, sorting_type
    selected_classes = request.form.getlist('selected_classes[]')
    rejected_classes = request.form.getlist('rejected_classes[]')
    sorting_type = request.form['sorting_type']


    print(selected_classes)
    print(rejected_classes)

    return jsonify({"message": "Sorting information saved successfully."}), 200


@app.route('/train', methods=['POST'])
def train_image_classifier():
    # Check if training is already in progress
    if TRAINING_IN_PROGRESS:
        return jsonify({"message": "Training is already in progress."}), 400

    # Set the training flag to indicate that training is in progress
    TRAINING_IN_PROGRESS = True

    try:
        class_count = int(request.form['class_count'])
        class_names = [
            request.form[f'class_name_{i}'] for i in range(class_count)]
        
        initial_epochs = int(request.form['initial_epochs'])
        finetune_epochs = int(request.form['finetune_epochs'])

        model_name = request.form['model_name']

        # Delete existing folders and create new ones
        delete_and_create_folders()

        # Process and save uploaded images for each class in separate folders
        for i, class_name in enumerate(class_names):
            class_folder = os.path.join(UPLOAD_FOLDER, class_name)
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
        mf.train_model(class_names, initial_epochs, finetune_epochs)

        # Create a dictionary to store class data
        class_data = {'class_names': class_names}

        # Create a config JSON file with class data
        config_file_path = os.path.join(MODEL_FOLDER, 'config.json')
        with open(config_file_path, 'w') as config_file:
            json.dump(class_data, config_file)

        # Create a zip file containing the trained model, class names, and config file
        model_zip_filename = f"{model_name}.zip"
        model_h5 = "static/models/model.h5"
        with zipfile.ZipFile(os.path.join(MODEL_FOLDER, model_zip_filename), 'w') as zipf:
            # Add config file to the zip
            zipf.write(config_file_path, 'config.json')
            zipf.write(model_h5, 'model.h5')  # Add model to the zip

        # Send a JSON response with the link to download the trained model
        response_data = {
            "message": "Model trained successfully",
            "download_url": f"/download/{model_zip_filename}"
        }
        return jsonify(response_data), 200
    except:
        print("An error occurred during training.")
        return jsonify({"message": "An error occurred during training."}), 500
    
    finally:
        # Reset the training flag when training is completed or an error occurs
        TRAINING_IN_PROGRESS = False


@app.route('/download/<filename>', methods=['GET'])
def download_file(filename):
    return send_file(os.path.join(MODEL_FOLDER, filename), as_attachment=True)


def find_cameras_until_num(num_cameras, max_cameras=20):
    camera_indices = []
    capture_objects = []

    if NUM_CAMERAS == 0:
        return camera_indices, capture_objects

    for i in range(max_cameras):
        capture = cv2.VideoCapture(i)
        if capture.isOpened():
            camera_indices.append(i)
            capture_objects.append(capture)
            if len(camera_indices) == num_cameras:
                break

    if len(camera_indices) != num_cameras:
        print(f"Required {num_cameras} cameras not found.")
        sys.exit(1)

    return camera_indices, capture_objects


def capture_and_save_images(camera_indices, max_attempts=5):
    file_names_to_check = []

    for i, camera_index in enumerate(camera_indices):
        success = False
        attempts = 0

        while not success and attempts < max_attempts:
            capture = cv2.VideoCapture(camera_index)
            ret, frame = capture.read()
            capture.release()  # Release the capture object immediately

            if ret:
                filename = f"camera_{i}.jpg"
                save_path = os.path.join("static/captured_images", filename)
                cv2.imwrite(save_path, frame)
                file_names_to_check.append(filename)
                success = True
            else:
                attempts += 1
                print("Failed")
                time.sleep(0.5)

    return file_names_to_check


def move_captured_images_to_unclassified_images():
    source_directory = "static/captured_images"
    destination_directory = "static/unclassified_images"

    # Get all captured images
    captured_images = os.listdir(source_directory)

    for image in captured_images:
        source_path = os.path.join(source_directory, image)
        destination_path = os.path.join(destination_directory, image)

        # Check if the file already exists in the destination directory
        if os.path.exists(destination_path):
            # If it exists, find a unique name
            base_name, ext = os.path.splitext(image)
            count = 1
            while os.path.exists(os.path.join(destination_directory, f"{base_name}_{count}{ext}")):
                count += 1
            new_image_name = f"{base_name}_{count}{ext}"
            destination_path = os.path.join(
                destination_directory, new_image_name)

        # Move the image to the destination directory
        shutil.move(source_path, destination_path)


def micro_controller_thread():
    freeze_support()

    mf.load_model(model_path="model.h5")

    try:
        ser = serial.Serial(PORT, 9600)
        print("Connected.")
    except:
        print("Not connected.")

    while True:
        try:
            data = ser.readline().decode().strip()  # Read data from the serial port
        except:
            print("Error reading from serial port.")

            # try to reconnect
            while True:	
                try:
                    ser = serial.Serial(PORT, 9600)
                    print("Connected.")
                    break
                except:
                    print("Not connected.")
                    time.sleep(1)

            continue
        if data == "":  # If the data is empty, wait for an "Enter" character
            continue

        move_captured_images_to_unclassified_images()
        print(f"Capturing {NUM_CAMERAS} images...")
        #file_names_to_check = capture_and_save_images(camera_indices)
        file_names_to_check = camera_manager.capture_and_save_images()
        time.sleep(1)
        update_websocket_images(file_names_to_check)



        # Initialize counters for selected and rejected predictions
        selected_count = 0
        rejected_count = 0

        for filename in file_names_to_check:
            print(filename)
            class_name, class_probability = mf.predict_image(filename, all_classes, model)
            print(f"{class_name}: {class_probability}%")
            print(f"Predicted {filename}: {class_name}")

            # Increment counters based on prediction
            if class_name in selected_classes:
                selected_count += 1
            if class_name in rejected_classes:
                rejected_count += 1

            # Handling for 'Dominant-Reject' and 'Dominant-Select' without needing to go through all images
            if sorting_type == 'dominant-reject' and class_name in rejected_classes:
                break
            if sorting_type == 'dominant-select' and class_name in selected_classes:
                break

        # Decision making based on sorting type
        decision = 'undecided'  # Initial state
        if sorting_type == 'average':
            if selected_count >= rejected_count:
                decision = 'selected'
            else:
                decision = 'rejected'
        elif sorting_type == 'dominant-select':
            decision = 'rejected' if selected_count == 0 else 'selected'
        else:  # 'dominant-reject'
            decision = 'selected' if rejected_count == 0 else 'rejected'

        # Execute actions based on the final decision
        if decision == 'selected':
            print("The object will not be sorted out.\n\r")
            ser.write("g\n\r".encode())
            update_websocket_text("don't sort out")
        else:
            print("The object will be sorted out.\n\r")
            ser.write("b\n\r".encode())
            update_websocket_text("sort out")

        print("Received data:", data)  # Assuming 'data' is defined and received elsewhere in your code.



if __name__ == '__main__':
    # Create the directorys if it doesn't exist
    os.makedirs("models", exist_ok=True)
    os.makedirs("static/captured_images", exist_ok=True)
    os.makedirs("static/unclassified_images", exist_ok=True)
    os.makedirs("static/models", exist_ok=True)
    os.makedirs("static/images", exist_ok=True)

    camera_indices, capture_objects = find_cameras_until_num(NUM_CAMERAS)
    print(camera_indices)

    camera_manager = camera.CameraManager(camera_indices)

    mc_thread = Thread(target=micro_controller_thread)
    mc_thread.daemon = True
    mc_thread.start()

    socketio.run(app, host="0.0.0.0", port=5000, debug=False, allow_unsafe_werkzeug=True)
