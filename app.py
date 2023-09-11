import os
import shutil
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'static/images'

def delete_and_create_folders():
    images_folder = app.config['UPLOAD_FOLDER']
    shutil.rmtree(images_folder)  # Delete the entire images folder
    os.makedirs(images_folder)    # Recreate the images folder

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/train', methods=['POST'])
def train_model():
    class_count = int(request.form['class_count'])
    class_names = [request.form[f'class_name_{i}'] for i in range(class_count)]

    # Delete existing folders and create new ones
    delete_and_create_folders()

    # Process and save uploaded images for each class in separate folders
    for i, class_name in enumerate(class_names):
        class_folder = os.path.join(app.config['UPLOAD_FOLDER'], class_name)
        os.makedirs(class_folder, exist_ok=True)  # Create class-specific folder if it doesn't exist

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

    # Now you can train your TensorFlow model using the organized data

    # Return a response or redirect to a results page
    return jsonify({"message": "Model trained successfully"})

if __name__ == '__main__':
    app.run(debug=True)
