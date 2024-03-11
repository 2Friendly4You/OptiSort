import cv2
import os
import threading
import time

class ContinuousCameraThread(threading.Thread):
    def __init__(self, camera_index, buffer, fps=10):
        threading.Thread.__init__(self)
        self.camera_index = camera_index
        self.buffer = buffer
        self.fps = fps
        self.running = True
        self.cap = cv2.VideoCapture(self.camera_index, cv2.CAP_DSHOW)
        # https://www.principiaprogramatica.com/2017/06/11/setting-manual-exposure-in-opencv/
        self.cap.set(cv2.CAP_PROP_EXPOSURE, -4)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv2.CAP_PROP_FPS, self.fps)

    def run(self):
        while self.running:
            if self.cap.isOpened():
                ret, frame = self.cap.read()
                if ret:
                    self.buffer[self.camera_index] = frame

    def stop(self):
        self.running = False
        if self.cap.isOpened():
            self.cap.release()

class CameraBufferManager:
    def __init__(self, camera_indices, fps=10):
        self.camera_indices = camera_indices
        self.buffer = {}
        self.threads = []
        self.fps = fps

    def start_capturing(self):
        for camera_index in self.camera_indices:
            thread = ContinuousCameraThread(camera_index, self.buffer, self.fps)
            thread.start()
            self.threads.append(thread)

    def get_latest_image(self, camera_index):
        if camera_index in self.buffer:
            return self.buffer[camera_index]
        else:
            return None
        
    def capture_and_save_images(self):
        file_names_to_check = []

        # get all the latest images from the buffer
        for i, camera_index in enumerate(self.camera_indices):
            latest_image = self.get_latest_image(camera_index)
            if latest_image is not None:
                filename = f"camera_{i}.jpg"
                save_path = os.path.join("static/captured_images", filename)
                cv2.imwrite(save_path, latest_image)
                file_names_to_check.append(save_path)

        return file_names_to_check

    def stop_all_capturing(self):
        for thread in self.threads:
            thread.stop()
        for thread in self.threads:
            thread.join()