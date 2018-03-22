#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
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
	protected:
		enum imageType{unknown, ascii, binary};
		bool openImage(const string&);
		bool ppmReadLine(fstream&, string&);
		unsigned char ppmReadPixel(fstream&);
	private:
		void init();
		deque<deque<float>> img;
		int height;
		int width;
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

Image::Image(const size_t& _length)
{
	init();
	return;
}

Image::Image(const size_t& _width, const size_t& _height)
{
	init();
	return;
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
		stream>>this->height>>this->width;
		cout<<"\t图片大小为:"<<this->height<<"×"<<this->width<<endl;
	}
	ppmReadLine(imageFile, strbuf);
	{
		stream>>this->colorDepth;
	}
	if (imgType == binary)
	{
		unsigned char pix;
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
	_fs>>pix>>pix>>pix;
	return pix;
}

void Image::init()
{
	this->height = 0;
	this->width = 0;
	this->colorDepth = 0;
	return;
}

int main(int argc, char* argv[])
{
	Image luoxiaohei("image/luoxiaohei_small_gray.ppm");
	Image luoxiaohei_c("image/luoxiaohei_small_xs_gray.ppm");
	return 0;
}