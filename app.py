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
UPLOAD_FOLDER = os.path.join("static", "images")

# Flag to track training status
TRAINING_IN_PROGRESS = False

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


@app.route('/documentation')
def documentation():
    return render_template('documentation.html')


@app.route('/get-unclassified-image/<image_name>', methods=['GET'])
def get_unclassified_image(image_name):
    return send_file(os.path.join("static", "unclassified_images", image_name))


@app.route('/upload-unclassified-images', methods=['POST'])
def upload_unclassified_images():
    if 'images' not in request.files:
        return jsonify({"message": "No file part"}), 400

    files = request.files.getlist('images')

    upload_folder = os.path.join("static", "unclassified_images")
    if not os.path.exists(upload_folder):
        os.makedirs(upload_folder)

    # Get all existing file names and convert them to integers
    existing_files = [f for f in os.listdir(upload_folder) if os.path.isfile(os.path.join(upload_folder, f))]
    existing_numbers = [int(f.split('.')[0]) for f in existing_files if f.split('.')[0].isdigit()]

    # Determine the next file number to use
    next_file_number = max(existing_numbers) + 1 if existing_numbers else 1

    for file in files:
        if file.filename == '':
            return jsonify({"message": "No selected file"}), 400
        if file:
            # Construct a new filename using the next available number
            new_filename = f"{next_file_number}.{file.filename.rsplit('.', 1)[1]}"
            file.save(os.path.join(upload_folder, new_filename))
            next_file_number += 1  # Increment the file number for the next file

    return jsonify({"message": "Files uploaded successfully."}), 200


@app.route('/get-unclassified-images', methods=['GET'])
def get_unclassified_images():
    # return the image names in the unclassified_images folder
    images = os.listdir(os.path.join("static", "unclassified_images"))

    if len(images) == 0:
        return jsonify({"message": "No unclassified images found."}), 404
    return jsonify(images), 200


@app.route('/remove-unclassified-image/<image_name>', methods=['DELETE'])
def remove_unclassified_image(image_name):
    os.remove(os.path.join("static", "unclassified_images", image_name))
    return jsonify({"message": "Image removed."}), 200


@app.route('/remove-all-unclassified-images', methods=['DELETE'])
def remove_all_unclassified_images():
    shutil.rmtree(os.path.join("static", "unclassified_images"))
    os.makedirs(os.path.join("static", "unclassified_images"))
    return jsonify({"message": "All unclassified images removed."}), 200


@app.route('/get-trained-models')
def current_settings_and_model():
    models_list = []
    models_dir = os.listdir("models")
    # Go through each model and get the config file and add it to the list of json to return
    for model in models_dir:
        config_file_path = os.path.join("models", model, "config.json")
        try:
            with open(config_file_path, 'r') as config_file:
                data = json.load(config_file)
                # Optionally, add the model name to the data if it's not already included
                data['model_name'] = model
                models_list.append(data)
        except FileNotFoundError:
            print(f"Config file not found for model: {model}")
        except json.JSONDecodeError:
            print(f"Error decoding JSON for model: {model}")

    return jsonify({"models": models_list}), 200


@app.route('/delete-model', methods=['DELETE'])
def delete_model():
    data = request.get_json()
    model_name = data['model_name']
    model_path = os.path.join("models", model_name)
    if os.path.exists(model_path):
        shutil.rmtree(model_path)
        return jsonify({"message": "Model deleted successfully."}), 200
    else:
        return jsonify({"message": "Model not found."}), 404


@app.route('/load-model', methods=['POST'])
def load_model():
    data = request.get_json()
    model_name = data['model_name']
    model_path = os.path.join("models", model_name)

    # load class names for rejection and selection
    global selected_classes
    global rejected_classes
    global sorting_type
    global all_classes

    # {"model_name": "Test", "class_count": "2", "initial_epochs": "20", "finetune_epochs": "20", "sorting_type": "dominant-reject", "class_names": ["Apples", "Banana"], "classes": [{"class_name": "Apples", "selected_or_rejected": "select", "images": ["00000013.JPG", "00000015.JPG", "00000017.JPG", "00000019.JPG"]}, {"class_name": "Banana", "selected_or_rejected": "reject", "images": ["00000014.JPG", "00000016.JPG", "00000018.JPG", "00000020.JPG"]}]}

    with open(os.path.join(model_path, "config.json"), 'r') as config_file:
        data = json.load(config_file)
        class_names = data['class_names']
        all_classes = class_names
        selected_classes = [c['class_name']
                            for c in data['classes'] if c['selected_or_rejected'] == "select"]
        rejected_classes = [c['class_name']
                            for c in data['classes'] if c['selected_or_rejected'] == "reject"]
        sorting_type = data['sorting_type']

        print(selected_classes)
        print(rejected_classes)
        print(sorting_type)
        print(all_classes)

    if os.path.exists(model_path):
        mf.load_model(model_path)
        return jsonify({"message": "Model loaded successfully."}), 200
    else:
        return jsonify({"message": "Model not found."}), 404


@app.route('/train', methods=['POST'])
def train_image_classifier():
    global TRAINING_IN_PROGRESS

    if TRAINING_IN_PROGRESS:
        return jsonify({"message": "Training is already in progress."}), 400

    data = request.get_json()
    model_name = data['model_name']

    # Validate received JSON data
    required_keys = ['class_count', 'class_names',
                     'initial_epochs', 'finetune_epochs', 'model_name', 'classes']
    if not all(key in data for key in required_keys):
        return jsonify({"message": "Missing data in request."}), 400

    # Extract class_names here to ensure it's defined for later use
    class_names = data['class_names']

    # Convert and validate integer fields
    try:
        class_count = int(data['class_count'])
        initial_epochs = int(data['initial_epochs'])
        finetune_epochs = int(data['finetune_epochs'])
    except ValueError:
        return jsonify({"message": "Invalid data format for numerical fields."}), 400

    # Validate class_names and classes structure
    if not isinstance(class_names, list) or not all(isinstance(cn, str) for cn in class_names):
        return jsonify({"message": "Invalid class_names format."}), 400
    if not isinstance(data['classes'], list) or not all(isinstance(c, dict) for c in data['classes']):
        return jsonify({"message": "Invalid classes format."}), 400

    # Check if model_name already exists
    model_path = os.path.join("models", model_name)
    if os.path.exists(model_path):
        return jsonify({"message": "A model with the same name already exists. Delete it from the home page."}), 400

    os.makedirs(model_path)
    TRAINING_IN_PROGRESS = True

    try:
        delete_and_create_folders()

        # Iterate over provided classes data
        for class_dict in data['classes']:
            class_name = class_dict['class_name']
            os.makedirs(os.path.join(UPLOAD_FOLDER, class_name), exist_ok=True)
            for image in class_dict['images']:
                source_path = os.path.join(
                    "static", "unclassified_images", image)
                destination_path = os.path.join(
                    UPLOAD_FOLDER, class_name, image)
                shutil.move(source_path, destination_path)

        # Example sleep to simulate long-running process
        sleep(4)

        # Example config file writing, adjust as necessary
        config_file_path = os.path.join(model_path, "config.json")
        with open(config_file_path, 'w') as config_file:
            json.dump(data, config_file)

        mf.train_model(class_names, os.path.join(
            model_path, "model.h5"), initial_epochs, finetune_epochs, socketio)

        # move images back to unclassified_images
        folder = UPLOAD_FOLDER
        for subfolder in os.listdir(folder):
            subfolder_path = os.path.join(folder, subfolder)
            if os.path.isdir(subfolder_path):
                for image in os.listdir(subfolder_path):
                    source_path = os.path.join(subfolder_path, image)
                    destination_path = os.path.join(
                        "static", "unclassified_images", image)
                    shutil.move(source_path, destination_path)

        TRAINING_IN_PROGRESS = False
        return jsonify({"message": "Model trained successfully, you can find it on the home page"}), 200
    except Exception as e:
        # If an error occurs, delete the model folder and return an error message
        if os.path.exists(model_path):
            shutil.rmtree(model_path)
        print(e)
        return jsonify({"message": "An error occurred during training."}), 500
    finally:
        TRAINING_IN_PROGRESS = False


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
        # file_names_to_check = capture_and_save_images(camera_indices)
        file_names_to_check = camera_manager.capture_and_save_images()
        time.sleep(1)
        update_websocket_images(file_names_to_check)

        # Initialize counters for selected and rejected predictions
        selected_count = 0
        rejected_count = 0

        for filename in file_names_to_check:
            print(filename)
            class_name, class_probability = mf.predict_image(
                filename, all_classes, model)
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

        # Assuming 'data' is defined and received elsewhere in your code.
        print("Received data:", data)


if __name__ == '__main__':
    # Create the directorys if it doesn't exist
    os.makedirs("models", exist_ok=True)
    os.makedirs("static/captured_images", exist_ok=True)
    os.makedirs("static/unclassified_images", exist_ok=True)
    os.makedirs("static/images", exist_ok=True)

    camera_indices, capture_objects = find_cameras_until_num(NUM_CAMERAS)
    print(camera_indices)

    camera_manager = camera.CameraManager(camera_indices)

    mc_thread = Thread(target=micro_controller_thread)
    mc_thread.daemon = True
    mc_thread.start()

    socketio.run(app, host="0.0.0.0", port=5000,
                 debug=False, allow_unsafe_werkzeug=True)
