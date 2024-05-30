import cv2
import numpy as np

def main():
    #Thermal image size
    size = 240 * 320

    # TODO: Change the file name
    file_name = "../sample_data/115616410.bin"

    with open(file_name, "rb") as thermal_file:
        thermal_data = np.fromfile(thermal_file, dtype=np.float32, count=size)

    float_image = thermal_data.reshape((240, 320))

    minVal, maxVal = float_image.min(), float_image.max()
    print(f"min value: {minVal} max value: {maxVal}")

    thermal_color = ((float_image - minVal) / (maxVal - minVal) * 255).astype(np.uint8)

    image = thermal_color

    cv2.imshow("image", image)

    cv2.waitKey()
    # TODO: Uncomment this if you want to save the image converted
    #cv2.imwrite("thermal_colored.png", image)

if __name__ == "__main__":
    main()
