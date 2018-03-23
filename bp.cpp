#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <cstdlib>

using namespace std;

class Image
{
	public:
		Image();
		Image(const string&);
		Image(const size_t&);
		Image(const size_t&, const size_t&);
		Image(const pair<size_t, size_t>&);
		Image(const pair<size_t, size_t>&, const size_t&);
		Image(const Image&);
		Image(const Image&, const bool&);
		Image convolution(const Image&, const bool&) const;
		const deque<deque<float>>& image() const;
		pair<size_t, size_t> imageSize() const;
		bool paste(const Image&, const size_t&, const size_t&);
		bool save(const string&);
	protected:
		enum imageType
		{
			unknown,
			ascii,
			binary
		};
		bool openImage(const string&);
		bool ppmReadLine(fstream&, string&);
		unsigned char ppmReadPixel(fstream&);
	private:
		void init();
		deque<deque<float>> img;
		size_t height;
		size_t width;
		int colorDepth;
};

Image::Image()
{
	init();
	return;
}

Image::Image(const string& _filename)
{
	init();
	if (!openImage(_filename))
	{
		cerr<<"打开文件"<<_filename<<"时遇到错误."<<endl;
		exit(-1);
	}
	else
	{
		cout<<"\t图片读取成功"<<endl;
	}
}

Image::Image(const size_t& _length):
	Image(_length, _length)
{
	return;
}

Image::Image(const size_t& _width, const size_t& _height)
{
	init();
	for (size_t imgHeight=0; imgHeight!=_height; imgHeight++)
	{
		deque<float> imgRow(_width, 0);
		this->img.push_back(imgRow);
	}
	this->width = _width;
	this->height = _height;
	return;
}

Image::Image(const pair<size_t, size_t>& _imgSize):
	Image(_imgSize, 0)
{
	return;
}

Image::Image(const pair<size_t, size_t>& _imgSize, const size_t& _padding):
	Image(_imgSize.first + _padding*2, _imgSize.second + _padding*2)
{
	return;
}

Image::Image(const Image& _img):
	Image(_img, false)
{
	return;
}

Image::Image(const Image& _img, const bool& _dontCopyContent)
{
	init();
	for (size_t imgHeight=0; imgHeight!=_img.imageSize().second; imgHeight++)
	{
		if (_dontCopyContent)
		{
			deque<float> imgRow(_img.imageSize().first, 0);
			this->img.push_back(imgRow);
		}
		else
		{
			deque<float> imgRow(_img.image()[imgHeight]);
			this->img.push_back(imgRow);
		}
	}
	this->width = _img.imageSize().first;
	this->height = _img.imageSize().second;
	return;
}

Image Image::convolution(const Image& _kernel, const bool& _padding = true) const
{
	Image result;
	if (_padding)
	{
		result = Image(this->imageSize());
		int paddingLength = (_kernel.imageSize().first-1)/2;
		Image paddedImage(this->imageSize(), paddingLength);
		paddedImage.paste(*this, paddingLength, paddingLength);
		for (size_t row=0; row!=result.imageSize().second; row++)
		{
			cerr<<"\r"<<row<<"/"<<result.imageSize().second;
			for (size_t col=0; col!=result.imageSize().first; col++)
			{
				float res = 0;
				for (size_t kernelRow = 0;
					kernelRow != _kernel.imageSize().second;
					kernelRow++)
				{
					for (size_t kernelCol = 0;
						kernelCol != _kernel.imageSize().first;
						kernelCol++)
					{
						res +=
							(paddedImage.image()[row+kernelRow][col+kernelCol]*
							_kernel.image()[kernelRow][kernelCol]);
					}
				}
				res /= _kernel.imageSize().first*_kernel.imageSize().second;
				result.img[row][col] = res;
			}
		}
		cerr<<endl;
	}
	else
	{
		
	}
	return result;
}

inline const deque<deque<float>>& Image::image() const
{
	return this->img;
}

inline pair<size_t, size_t> Image::imageSize() const
{
	return pair<size_t, size_t>(this->width, this->height);
}

bool Image::paste(const Image& _sourceImg, const size_t& _widthLocation,
	const size_t& _heightLocation)
{
	for (size_t height = 0;
		height != _sourceImg.imageSize().second;
		height++)
	{
		for (size_t width = 0;
			width != _sourceImg.imageSize().first;
			width++)
		{
			this->img[height + _heightLocation][width + _widthLocation] =
				_sourceImg.img[height][width];
		}
	}
	return true;
}

bool Image::save(const string& _filename)
{
	fstream imageFile(_filename, ios::out);
	imageFile<<"P6\n# Created By dgideas@outlook.com\n";
	imageFile<<this->width<<" "<<this->height<<"\n255\n";
	for (auto& imgRow: this->img)
	{
		for (auto& pix: imgRow)
		{
			unsigned char p = pix;
			imageFile<<p<<p<<p;
		}
	}
}

bool Image::openImage(const string& _filename)
{
	fstream imageFile(_filename, ios::in);
	imageType imgType = unknown;
	string strbuf;
	stringstream stream;
	if (!imageFile.is_open())
	{
		return false;
	}
	ppmReadLine(imageFile, strbuf);
	if (strbuf == "P3")
	{
		imgType = ascii;
		cout<<"使用ASCII方式读取"<<_filename<<endl;
	}
	else if(strbuf == "P6")
	{
		imgType = binary;
		cout<<"使用二进制方式读取"<<_filename<<endl;
	}
	ppmReadLine(imageFile, strbuf);
	{
		stream<<strbuf;
		stream>>this->width>>this->height;
		cout<<"\t图片大小为:"<<this->width<<"×"<<this->height<<endl;
	}
	ppmReadLine(imageFile, strbuf);
	{
		stream>>this->colorDepth;
	}
	if (imgType == binary)
	{
		for (size_t pixHeight=0; pixHeight!=this->height; pixHeight++)
		{
			deque<float> imgRow(this->width, 0);
			for (size_t pixWidth=0; pixWidth!=this->width; pixWidth++)
			{
				imgRow[pixWidth] = ppmReadPixel(imageFile);
			}
			this->img.push_back(imgRow);
		}
	}
	return true;
}

bool Image::ppmReadLine(fstream& _fs, string& _strbuf)
{
	do
	{
		if (!getline(_fs, _strbuf))
		{
			return false;
		}
	}
	while(_strbuf.find('#') != -1);
	return true;
}

unsigned char Image::ppmReadPixel(fstream& _fs)
{
	unsigned char pix;
	pix = _fs.get();
	pix = _fs.get();
	pix = _fs.get();
	return pix;
}

void Image::init()
{
	this->img.clear();
	this->height = 0;
	this->width = 0;
	this->colorDepth = 0;
	return;
}

int main(int argc, char* argv[])
{
	Image luoxiaohei("image/luoxiaohei_small_gray.ppm");
	Image luoxiaohei_c("image/luoxiaohei_small_xs_gray.ppm");
	Image cres = luoxiaohei.convolution(luoxiaohei_c);
	cres.save("image/output.ppm");
	return 0;
}