# Lab 2: Digital Image Processing

## Лабораторная работа 2: Основы обработки цифровых изображений

This application implements various digital image processing algorithms including:

### Implemented Features

#### 1. Histogram Operations and Contrast Enhancement
- **Histogram Building and Equalization**: Calculate and display image histograms
- **Histogram Equalization (Grayscale)**: Equalize histogram for grayscale images
- **Histogram Equalization (RGB)**: Equalize each RGB channel independently
- **Histogram Equalization (HSV)**: Equalize only the V (Value) channel in HSV color space
- **Linear Contrast Enhancement**: Adjust contrast using alpha (contrast) and beta (brightness) parameters
- **Automatic Linear Contrast**: Automatically stretch histogram to full range

#### 2. Low-pass Filters (Smoothing/Denoising)
- **Average Filter**: Simple averaging filter for noise reduction
- **Gaussian Filter**: Weighted averaging with Gaussian kernel (adjustable sigma and kernel size)

#### 3. High-pass Filters (Sharpening)
- **Sharpen Filter**: Basic sharpening using Laplacian-based kernel
- **Laplacian Filter**: Edge enhancement using Laplacian operator
- **Unsharp Masking**: Advanced sharpening technique with adjustable amount

#### 4. Non-linear Filters (Order Statistics)
- **Median Filter**: Excellent for salt-and-pepper noise removal
- **Min Filter**: Erosion-like filter
- **Max Filter**: Dilation-like filter

#### 5. Global Thresholding
- **Otsu's Method**: Automatic threshold selection using variance maximization
- **Fixed Threshold**: Manual threshold selection
- **Mean Threshold**: Threshold based on mean pixel value

#### 6. Local/Adaptive Thresholding
- **Adaptive Threshold**: Local mean-based thresholding with adjustable parameters
- **Niblack's Method**: Local thresholding using mean and standard deviation
- **Sauvola's Method**: Improved version of Niblack for document images

#### 7. Edge Detection and Segmentation
- **Sobel Edge Detection**: Gradient-based edge detection
- **Prewitt Edge Detection**: Alternative gradient operator
- **Canny Edge Detection**: Multi-stage edge detection with hysteresis thresholding

#### 8. Morphological Operations
- **Erosion**: Shrink objects
- **Dilation**: Expand objects
- **Opening**: Remove small objects (erosion + dilation)
- **Closing**: Fill small holes (dilation + erosion)
- **Structuring Elements**: Rectangle, Cross, and Ellipse shapes

## Building the Application

### Requirements
- CMake 3.19 or higher
- C++17 compatible compiler
- OpenGL 4.6
- GLFW3
- GLM
- vcpkg (recommended for dependency management)

### Build Instructions

```bash
cd lab2
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build .
./lab2
```

## Usage

1. **Loading Images**: Click "Load Image" button and enter the path to an image file
   - Supported formats: PNG, JPG, BMP
   - Test images are provided in `test_images/` directory

2. **Selecting Operations**: Choose an operation from the dropdown menu
   - Each operation has specific parameters that can be adjusted

3. **Adjusting Parameters**: Use sliders and input fields to fine-tune the algorithm parameters
   - Parameters change dynamically based on selected operation

4. **Viewing Results**: 
   - Original image is displayed on the left
   - Processed image is displayed on the right
   - Enable "Show Histogram" to view histogram of processed image

5. **Saving Images**: Click "Save Image" button to save the processed result

## Test Images

The `test_images/` directory contains various test images:
- `test.png` - Simple geometric shapes for basic testing
- `low_contrast.png` - Low contrast image for histogram equalization
- `noisy.png` - Image with salt-and-pepper noise for filter testing
- `blurred.png` - Blurred image for sharpening filter testing

## Algorithm Explanations

### Histogram Equalization
Redistributes pixel intensities to achieve uniform histogram distribution, enhancing contrast especially in low-contrast images.

**Comparison of RGB vs HSV approach**:
- **RGB**: Equalizes each color channel independently, may cause color shifts
- **HSV**: Equalizes only brightness (V channel), preserves original colors better

### Thresholding Methods
**Global vs Local**:
- **Global**: Single threshold for entire image, faster but less adaptive
- **Local**: Different threshold for each region, better for varying illumination

**Otsu vs Adaptive**:
- **Otsu**: Automatic, optimal for bimodal histograms
- **Adaptive/Niblack/Sauvola**: Better for document images with varying background

### Morphological Operations
- **Opening**: Good for removing noise while preserving object size
- **Closing**: Good for filling holes and connecting nearby objects

## Implementation Notes

- All algorithms work with both grayscale and color images
- Color images are automatically converted to grayscale for single-channel operations
- Boundary pixels are handled using edge replication
- Performance is not optimized for real-time processing of large images

## Grading Criteria (100 points)

- ✅ **Correct implementation of methods** (50 points)
- ✅ **Selection of appropriate test images** (20 points)
- ✅ **Explanations and documentation** (30 points)

## Author

Implementation for Computer Graphics course, Lab 2.
