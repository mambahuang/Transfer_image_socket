from PIL import Image
import numpy as np

def image_to_hex(image_path):
    # Open the image
    img = Image.open(image_path)
    
    # Convert image to grayscale (if not already)
    img = img.convert('L')
    
    # Get pixel values and convert to hex
    pixels = np.array(img)
    hex_values = np.vectorize(lambda x: f'{x:02x}')(pixels)
    
    return hex_values

def pad_image_center(hex_values, target_width, target_height, pad_value='00'):
    original_height, original_width = hex_values.shape
    padded_image = np.full((target_height, target_width), pad_value, dtype='<U2')
    
    # Calculate the starting position to center the image
    start_x = (target_width - original_width) // 2
    start_y = (target_height - original_height) // 2
    
    # Place the original image's hex values into the new image
    padded_image[start_y:start_y + original_height, start_x:start_x + original_width] = hex_values
    
    return padded_image

def save_hex_image(hex_image, output_path):
    # Convert hex values back to integers for image saving
    int_image = np.vectorize(lambda x: int(x, 16))(hex_image)
    
    # Create and save the image
    img = Image.fromarray(int_image.astype(np.uint8))
    img.save(output_path)

# Paths
input_image_path = './img/output.png'
output_image_path = './img/image_padding.png'

# Convert the original image to hex
hex_values = image_to_hex(input_image_path)

# Pad the hex values to 1920x1080 centered
padded_hex_image = pad_image_center(hex_values, 1920, 1080)

# Save the padded hex image
save_hex_image(padded_hex_image, output_image_path)

print("Image processing complete!")
