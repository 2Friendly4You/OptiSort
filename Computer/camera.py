import cv2
import os
import time

class CameraThread:
    def __init__(self, camera_index):
        self.camera_index = camera_index

    def capture_frame(self):
        frame = None
        cap = cv2.VideoCapture(self.camera_index, cv2.CAP_DSHOW)
        time.sleep(0.1)  # Allow some time for camera initialization

        if cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                frame = None

        cap.release()
        return frame

class CameraManager:
    def __init__(self, camera_indices):
        self.camera_indices = camera_indices

    def capture_and_save_images(self, max_attempts=5):
        file_names_to_check = []

        for i, camera_index in enumerate(self.camera_indices):
            camera_thread = CameraThread(camera_index)
            success = False
            attempts = 0

            while not success and attempts < max_attempts:
                frame = camera_thread.capture_frame()
                if frame is not None:
                    filename = f"camera_{i}.jpg"
                    save_path = os.path.join("static/captured_images", filename)
                    cv2.imwrite(save_path, frame)
                    print(save_path)
                    file_names_to_check.append(save_path)
                    success = True
                else:
                    attempts += 1
                    print(f"Failed to capture from camera {i}, attempt {attempts}")
                    time.sleep(0.5)

        return file_names_to_check