import cv2
import threading
import os
import time

class CameraThread(threading.Thread):
    def __init__(self, camera_index):
        super().__init__()
        self.camera_index = camera_index
        self.cap = cv2.VideoCapture(camera_index, cv2.CAP_DSHOW)
        self.latest_frame = None
        self.running = True

    def run(self):
        while self.running:
            ret, frame = self.cap.read()
            if ret:
                self.latest_frame = frame
            time.sleep(0.1)  # Adjust sleep time as needed

    def get_frame(self):
        return self.latest_frame

    def stop(self):
        self.running = False
        self.cap.release()

class CameraManager:
    def __init__(self, camera_indices):
        self.camera_threads = [CameraThread(index) for index in camera_indices]
        for thread in self.camera_threads:
            thread.start()

    def capture_and_save_images(self, max_attempts=5):
        file_names_to_check = []
        for i, thread in enumerate(self.camera_threads):
            success = False
            attempts = 0

            while not success and attempts < max_attempts:
                frame = thread.get_frame()
                if frame is not None:
                    filename = f"camera_{i}.jpg"
                    save_path = os.path.join("static/captured_images", filename)
                    cv2.imwrite(save_path, frame)
                    file_names_to_check.append(save_path)
                    success = True
                else:
                    attempts += 1
                    print(f"Failed to capture from camera {i}, attempt {attempts}")
                    time.sleep(0.5)

        return file_names_to_check

    def stop_all_cameras(self):
        for thread in self.camera_threads:
            thread.stop()
        for thread in self.camera_threads:
            thread.join()

# Example usage
# camera_manager = CameraManager([0, 1, 2])  # Initialize with camera indices
# file_paths = camera_manager.capture_and_save_images()
# camera_manager.stop_all_cameras()
