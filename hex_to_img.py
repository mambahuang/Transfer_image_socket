from PIL import Image
import numpy as np

f1 = open('./dat/ciphertext_b.dat', 'r')
f2 = open('./dat/ciphertext_g.dat', 'r')
f3 = open('./dat/ciphertext_r.dat', 'r')

hex_tmp = []
hex_b = []
for line in f1:
    for idx in range(0, len(line) - 2, 2):
        hex_tmp.append(line[idx:idx+2])
    hex_b.append(hex_tmp)
    hex_tmp = []

print(hex_b[0:5])
print(len(hex_b))

hex_tmp = []
hex_g = []
for line in f2:
    for idx in range(0, len(line) - 2, 2):
        hex_tmp.append(line[idx:idx+2])
    hex_g.append(hex_tmp)
    hex_tmp = []

# print(hex_g[0:5])
# print(len(hex_g))

hex_tmp = []
hex_r = []
for line in f3:
    for idx in range(0, len(line) - 2, 2):
        hex_tmp.append(line[idx:idx+2])
    hex_r.append(hex_tmp)
    hex_tmp = []

# print(hex_r[0:5])
# print(len(hex_r))

hex_arr_b = np.array(hex_b)
hex_arr_g = np.array(hex_g)
hex_arr_r = np.array(hex_r)

def hex_to_image(hex_values, output_path):
    # Convert hex values to integers
    int_values = np.vectorize(lambda x: int(x, 16))(hex_values)
    
    # Create an image from the integer values
    img = Image.fromarray(int_values.astype(np.uint8))
    
    # Save the image
    img.save(output_path)

# Example 640x480 hex values array (replace this with your actual hex values)
hex_values_b = np.full((1080, 1920), '00', dtype='<U2')  # Example: filled with hex value '00'
hex_values_g = np.full((1080, 1920), '00', dtype='<U2')  # Example: filled with hex value '00'
hex_values_r = np.full((1080, 1920), '00', dtype='<U2')  # Example: filled with hex value '00'

# Place the 640x480 hex values array at the center of the 1920x1080 hex values array
row_b = 0
col_b = 0
lenRow_b = len(hex_arr_b)
lenCol_b = len(hex_arr_b[0])
for i in range(1080):
    for j in range(1920):
        hex_values_b[i][j] = hex_arr_b[row_b][col_b]
        if col_b == lenCol_b - 1:
            row_b += 1
            col_b = 0
        else:
            col_b += 1

row_g = 0
col_g = 0
lenRow_g = len(hex_arr_g)
lenCol_g = len(hex_arr_g[0])
for i in range(1080):
    for j in range(1920):
        hex_values_g[i][j] = hex_arr_g[row_g][col_g]
        if col_g == lenCol_g - 1:
            row_g += 1
            col_g = 0
        else:
            col_g += 1

row_r = 0
col_r = 0
lenRow_r = len(hex_arr_r)
lenCol_r = len(hex_arr_r[0])
for i in range(1080):
    for j in range(1920):
        hex_values_r[i][j] = hex_arr_r[row_r][col_r]
        if col_r == lenCol_r - 1:
            row_r += 1
            col_r = 0
        else:
            col_r += 1

# Output path for the generated image
output_image_path_b = 'output_image_b.png'
output_image_path_g = 'output_image_g.png'
output_image_path_r = 'output_image_r.png'

# Convert hex values to image and save
hex_to_image(hex_values_b, output_image_path_b)
hex_to_image(hex_values_g, output_image_path_g)
hex_to_image(hex_values_r, output_image_path_r)

print("Image created and saved successfully!")
