import time
import math
import cv2
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
from pythonosc.udp_client import SimpleUDPClient

# Wekinator listens here
client = SimpleUDPClient("127.0.0.1", 6448)

# load the hand tracking model (hand_landmarker.task must be in the same folder)
options = vision.HandLandmarkerOptions(
    base_options=python.BaseOptions(model_asset_path="hand_landmarker.task"),
    running_mode=vision.RunningMode.VIDEO,
    num_hands=1,
)
landmarker = vision.HandLandmarker.create_from_options(options)

cap = cv2.VideoCapture(0)   # 0 is the built in webcam

while cap.isOpened():
    ok, frame = cap.read()
    if not ok:
        break

    frame = cv2.flip(frame, 1)   # mirror image so movement feels natural
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)
    result = landmarker.detect_for_video(mp_image, int(time.time() * 1000))

    h, w, _ = frame.shape

    if result.hand_landmarks:
        hand = result.hand_landmarks[0]

        # the wrist (point 0) becomes the origin, so moving the whole hand
        # around the frame does not change the values
        wrist = hand[0]

        # hand size: distance from wrist to the base of the middle finger (point 9)
        # dividing by it means closer or further from the camera does not matter
        size = math.hypot(hand[9].x - wrist.x, hand[9].y - wrist.y)
        if size == 0:
            size = 1

        values = []
        for point in hand:
            values += [(point.x - wrist.x) / size, (point.y - wrist.y) / size]
            cv2.circle(frame, (int(point.x * w), int(point.y * h)), 4, (0, 255, 0), 2)

        cv2.putText(frame, "hand found", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
    else:
        values = [0.0] * 42   # no hand: send zeros
        cv2.putText(frame, "no hand", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)

    client.send_message("/wek/inputs", values)

    cv2.imshow("Hand gestures (press q to quit)", frame)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
