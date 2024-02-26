import cv2
import os
import time
import threading

class CameraThread(threading.Thread):
    def __init__(self, camera_index, output_list, index, max_attempts=5):
        threading.Thread.__init__(self)
        self.camera_index = camera_index
        self.output_list = output_list
        self.index = index
        self.max_attempts = max_attempts

    def run(self):
        attempts = 0
        success = False
        while not success and attempts < self.max_attempts:
            cap = cv2.VideoCapture(self.camera_index, cv2.CAP_DSHOW)
            time.sleep(0.1)  # Allow some time for camera initialization

            if cap.isOpened():
                ret, frame = cap.read()
                if ret:
                    filename = f"camera_{self.index}.jpg"
                    save_path = os.path.join("static/captured_images", filename)
                    cv2.imwrite(save_path, frame)
                    self.output_list.append(save_path)
                    success = True
                else:
                    print(f"Failed to capture from camera {self.index}, attempt {attempts}")
            cap.release()
            attempts += 1
            if not success:
                time.sleep(0.5)

class CameraManager:
    def __init__(self, camera_indices):
        self.camera_indices = camera_indices

    def capture_and_save_images(self):
        threads = []
        file_names_to_check = []

        for i, camera_index in enumerate(self.camera_indices):
            thread = CameraThread(camera_index, file_names_to_check, i)
            threads.append(thread)
            thread.start()

        for thread in threads:
            thread.join()

        return file_names_to_check
