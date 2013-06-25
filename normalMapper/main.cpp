//
//  main.cpp
//
//     _____    ___
//    /    /   /  /     normalMapper
//   /  __/ * /  /__    (c) ponies & light, 2013. All rights reserved.
//  /__/     /_____/    poniesandlight.co.uk
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  Created by tim on 13/06/2013.
//


/*

	NOTE:	Make sure to set the correct commandline arguments
			if you run this from within XCode.
	
	GO TO:	Edit Scheme -> Arguments -> Arguments Passed on Launch

	You will have to edit the -i and -o arguments to reflect the correct path to
	your input and output file paths, and the -w and -h options to have the
	correct width and height for your input image should these be different from
	the default values w=16384 and h=8192
 
*/


#include <iostream>
#include <Accelerate/Accelerate.h>
#include <iostream>
#include <fstream>

using namespace std;

ifstream::pos_type size;
char * memblock __attribute__ ((aligned));

// GLOBALS

unsigned long IMG_WIDTH=16384;
unsigned long IMG_HEIGHT=8192;
string inputFilePath;
string outputFilePath;
int kernelType = 0;
bool WRAP_LR=TRUE;	// calculate left vertical line and right vertical line by wrapping around edges
bool WRAP_TB=FALSE;	// calculate top horizontal line and bottom horizontal line by wrapping around edges

void MyInitBuffer( vImage_Buffer *result, int width, int height, size_t bytesPerPixel )
{
	size_t rowBytes = width * bytesPerPixel;
	
	//Set up the buffer
	result->height = height;
	result->width = width;
	result->rowBytes = rowBytes;
	result->data = malloc( rowBytes * height );
}

void MyFreeBuffer( vImage_Buffer *buffer )
{
	if( buffer && buffer->data )
		free( buffer->data );
}


void convolveEdgePixels(vImage_Buffer& buffer_in, vImage_Buffer& buffer_out, float* kernel){
	// invariants
	// width : IMG_WIDTH,
	// height : IMG_HEIGHT
	// kernel size : 3x3
	
	long x=0, y = 0;
	long pixel_index;
	
	if (WRAP_LR){
		
		for (int i = 1; i<IMG_HEIGHT-1; i++){
			x = 0;
			y = i;
			float pixel_value = 0.0;
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[0];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[1];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[2];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[3];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[4];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[5];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[6];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[7];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[8];
			
			pixel_value *= 1.0/9.0;
			
			// write the value to the output buffer.
			static_cast<float * >(buffer_out.data)[x + y * IMG_WIDTH] = pixel_value;
		}
		
		
		for (int i = 1; i<IMG_HEIGHT-1; i++){
			x = IMG_WIDTH-1;
			y = i;
			float pixel_value = 0.0;
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[0];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[1];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[2];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[3];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[4];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[5];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[6];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[7];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[8];
			
			pixel_value *= 1.0/9.0;
			
			// write the value to the output buffer.
			static_cast<float * >(buffer_out.data)[x + y * IMG_WIDTH] = pixel_value;
		}
	}
	
	if (WRAP_TB){
		
		// now do the horizontal lines ...
		
		for (int i = 0; i<IMG_WIDTH; i++){
			x = i;
			y = 0;
			float pixel_value = 0.0;
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[0];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[1];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[2];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[3];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[4];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[5];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[6];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[7];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[8];
			
			pixel_value *= 1.0/9.0;
			
			// write the value to the output buffer.
			static_cast<float * >(buffer_out.data)[x + y * IMG_WIDTH] = pixel_value;
		}
		
		for (int i = 0; i<IMG_WIDTH; i++){
			x = i;
			y = IMG_HEIGHT-1;
			float pixel_value = 0.0;
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[0];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[1];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y-1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[2];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[3];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[4];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + ((y  ) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[5];
			
			pixel_index = (x-1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[6];
			pixel_index = (x   + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[7];
			pixel_index = (x+1 + IMG_WIDTH) % IMG_WIDTH + (((y+1 +IMG_HEIGHT) % IMG_HEIGHT) * IMG_WIDTH);	pixel_value += static_cast<float * >(buffer_in.data)[pixel_index] * kernel[8];
			
			pixel_value *= 1.0/9.0;
			
			// write the value to the output buffer.
			static_cast<float * >(buffer_out.data)[x + y * IMG_WIDTH] = pixel_value;
		}
	}
	
}


void barf(){
	std::cout << "Usage: ./normalMapper -i infile -o outfile -w WIDTH -h HEIGHT {-k [0,1,2] -Wh [TRUE|FALSE] -Wv [TRUE|FALSE]}\n# Optional Argument -k is for convolution kernels: 0:sobel(default), 1:scharr, 2:prewitt";
}


int main(int argc, const char * argv[])
{
	for ( int i=1; i<argc; i++){
		if (i + 1 != argc) { // Check that we haven't finished parsing already
			if (strcmp(argv[i],"-i") == 0) {
				// We know the next argument *should* be the filename:
				inputFilePath = argv[i + 1];
				i++;
			} else if (strcmp(argv[i],"-o") == 0) {
				outputFilePath = argv[i + 1];
				i++;
			} else if (strcmp(argv[i],"-w")==0) {
				IMG_WIDTH = atoi(argv[i + 1]);
				i++;
			} else if (strcmp(argv[i],"-h")==0) {
				IMG_HEIGHT = atoi(argv[i + 1]);
				i++;
			} else if (strcmp(argv[i],"-k")==0) {
				kernelType = atoi(argv[i + 1]);
				i++;
			} else if (strcmp(argv[i],"-Wlr")==0) {
				// horizontal wrap on
				WRAP_LR = (strcmp(argv[i+1], "TRUE") == 0) ? true : false;
				i++;
			} else if (strcmp(argv[i],"-Wtb")==0) {
				WRAP_TB = (strcmp(argv[i+1], "TRUE") == 0) ? true : false;
				i++;
			} else {
				barf();
				exit(1);
			}
		}
	}

	if ( inputFilePath == "" || outputFilePath == "" || IMG_WIDTH == 0 || IMG_HEIGHT == 0 || kernelType < 0 || kernelType > 2) {
		barf();
		return 1;
	}
	
	
	// insert code here...
	
	string kernelNames[] = {"sobel", "scharr", "prewitt"};
	
	std::cout << "arguments: " << endl <<
	"inputFilePath: " << inputFilePath << endl <<
	"outputFilePath: " << outputFilePath << endl <<
	"Image width: " << IMG_WIDTH << endl <<
	"Image height: " << IMG_HEIGHT << endl <<
	"Kernel Type: " << kernelNames[kernelType] << endl <<
	"Wrap convolution left-right: " << (WRAP_LR ? "true" : "false") << endl <<
	"Wrap convolution top-bottom: " << (WRAP_TB ? "true" : "false") << endl;

	std::cout << endl;
	std::cout << "Starting up." << endl;

	

	ifstream file (inputFilePath, ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{

		size = file.tellg();
		memblock = new char __attribute__ ((aligned)) [size];
		
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();

		cout << "file size: " << size << endl;
		cout << "the complete file content is in memory" << endl;

		// create srcBuf

		vImage_Buffer srcBuf;
		MyInitBuffer(&srcBuf, IMG_WIDTH, IMG_HEIGHT , 2);
		// copy planar shorts to srcBuf
		memcpy(srcBuf.data, memblock, size) ;
		
		// we don't need memblock anymore:
		delete [] memblock;
		
		// now create a buffer that will hold our data as floats
		
		vImage_Buffer dstBuf;
		MyInitBuffer(&dstBuf, IMG_WIDTH, IMG_HEIGHT , 4);

		// convert from unsigned short to floats
		vImage_Error err = vImageConvert_16UToF(&srcBuf, &dstBuf, 0, 1.0, kvImageNoFlags);
		cout << "Converted to float: " << err << endl;

		// we don't need srcBuf anymore.
		MyFreeBuffer(&srcBuf);
		

		vImage_Buffer sobelDstBufFx;		// create a buffer that will hold our x-Sobel result.
		MyInitBuffer(&sobelDstBufFx, IMG_WIDTH, IMG_HEIGHT , 4);

		vImage_Buffer sobelDstBufFy;		// create a buffer that will hold our y-Sobel result.
		MyInitBuffer(&sobelDstBufFy, IMG_WIDTH, IMG_HEIGHT , 4);

		// sobel kernel
		float kernelDxSobel[] = {1, 0, -1, 2, 0, -2, 1, 0, -1};  // dx
		float kernelDySobel[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};  // dy
		
		// scharr kernel
		float kernelDxScharr[] = {3, 10, 3, 0, 0, 0, -3, -10, -3};  // dx
		float kernelDyScharr[] = {3, 0, -3, 10, 0, -10, 3, 0, -3};  // dy
		
		
		// Prewitt kernel
		float kernelDxPrewitt[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};  // dx
		float kernelDyPrewitt[] = {1, 1, 1, 0, 0, 0, -1, -1, -1};  // dy
		
		
		float * kernelDx;
		float * kernelDy;

		
		switch (kernelType) {
			case 1:	// 1
				kernelDx = kernelDxScharr;
				kernelDy = kernelDyScharr;
				break;
			case 2:	// 2
				kernelDx = kernelDxPrewitt;
				kernelDy = kernelDyPrewitt;
				break;
			default: // 0
				kernelDx = kernelDxSobel;
				kernelDy = kernelDySobel;
				break;
		}

		
		uint32_t kernel_height = 3;
		uint32_t kernel_width = 3;
		Pixel_F backgroundColor = 0.5;

		// apply sobel kernel dx
		err = vImageConvolve_PlanarF( &dstBuf, &sobelDstBufFx, NULL, 0, 0, kernelDx, kernel_height, kernel_width, backgroundColor, kvImageCopyInPlace);
		cout << "Applied Sobel kernel dx: " << err << endl;
		
		convolveEdgePixels(dstBuf, sobelDstBufFx, kernelDx);
		cout << "Convolved edge pixels Dx. " << endl;
		
		// apply sobel kernel dy
		err = vImageConvolve_PlanarF( &dstBuf, &sobelDstBufFy, NULL, 0, 0, kernelDy, kernel_height, kernel_width, backgroundColor, kvImageCopyInPlace);
		cout << "Applied Sobel kernel dx: " << err << endl;

		convolveEdgePixels(dstBuf, sobelDstBufFy, kernelDy);
		cout << "Convolved edge pixels Dy. " << endl;

		// free our source buffer now what the convolutions have been calculated.
		MyFreeBuffer(&dstBuf);

		vImage_Buffer blueBuf;		// create a buffer that will our blue channel.
		MyInitBuffer(&blueBuf, IMG_WIDTH, IMG_HEIGHT , 4);
		cout << "Created blue channel" << err << endl;
		float blueValue = 32768.0;
		// fill this buffer with uniform blue.
		vDSP_vfill(&blueValue, static_cast<float*>(blueBuf.data), 1, IMG_WIDTH*IMG_HEIGHT);
		cout << "Filled blue channel with uniform 1.0f" << endl;

		// create a buffer that will hold our final RGB image.
		vImage_Buffer rgbFImage= {	malloc(IMG_WIDTH*IMG_HEIGHT * 4 * 3),	IMG_HEIGHT,	IMG_WIDTH,	IMG_WIDTH * 4 * 3};	// data, height, width, rowBytes
		
		// create an intermediate image.
		
		err = vImageConvert_PlanarFtoRGBFFF(&sobelDstBufFx,
									  &sobelDstBufFy,
									  &blueBuf,
									  &rgbFImage,
									  kvImageNoFlags);
		
		cout << "Created interleaved RGBFFF buffer" << err << endl;

		MyFreeBuffer(&sobelDstBufFx);
		MyFreeBuffer(&sobelDstBufFy);
		
		vImage_Buffer charDstBuf = {	malloc(IMG_WIDTH*IMG_HEIGHT * 2 * 3), IMG_HEIGHT, IMG_WIDTH * 3, IMG_WIDTH * 2 * 3}; // data, height, width, rowBytes
		
		// we need to adapt the buffer size here, so that 
		rgbFImage.width *=3;
		// now we can convert the file to 16U
		err = vImageConvert_FTo16U(&rgbFImage, &charDstBuf, -327.68, 0.01, kvImageNoFlags);
		cout << "Converted back to 16U: " << err << endl;
		
		
		MyFreeBuffer(&rgbFImage);
		
		cout << "Start target file write-out..." << endl;
		std::ofstream outfile (outputFilePath, std::ofstream::binary);
		// write to outfile
		outfile.write(static_cast<char*>(charDstBuf.data),size*3);
		
		outfile.close();
		
		
		MyFreeBuffer(&charDstBuf);
		
	}
	else cout << "Unable to open file";

    cout<< "Finished." << endl;
	return 0;
}


