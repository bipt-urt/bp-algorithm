// dgideas@outlook.com
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <cstdlib>
#include <random>
#include <algorithm>

using namespace std;

class Image
{
	public:
		using contentType = float;
		Image();
		Image(const string&);
		Image(const size_t&);
		Image(const size_t&, const size_t&);
		Image(const pair<size_t, size_t>&);
		Image(const pair<size_t, size_t>&, const size_t&);
		Image(const Image&);
		Image(const Image&, const bool&);
		Image(const Image&, const char*);
		Image(const Image&, const string&);
		Image convolution(const Image&, const bool&) const;
		const deque<deque<float>>& image() const;
		pair<size_t, size_t> imageSize() const;
		Image operator-(const Image&);
		bool paste(const Image&, const size_t&, const size_t&);
		bool save(const string&);
		void minMaxNormalization(const contentType&, const contentType&);
		void minMaxNormalization(contentType, contentType,
			contentType, contentType);
		contentType trainingKernel(
			const Image&, const Image&, const contentType&);
		void dump() const;
		void dump(const string&) const;
		void changeImageName(const string&);
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
		string imageName;
		bool isDebug;
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
		this->imageName = _filename;
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
	this->imageName = _img.imageName;
	return;
}

Image::Image(const Image& _img, const char* _str):
	Image(_img, std::string(_str))
{
	return;
}

Image::Image(const Image& _img, const string& _constructType)
{
	init();
	if (_constructType == "random")
	{
		unsigned int randomSeed;
		if (this->isDebug)
		{
			randomSeed = 24683;
			// randomSeed = 1368975462;
		}
		else
		{
			random_device rd;
			randomSeed = rd();
		}
		cout<<"rd:"<<randomSeed<<endl;
		mt19937 mt(randomSeed);
		uniform_int_distribution<> dist(0, 255);
		for (size_t row = 0;
			row != _img.imageSize().second;
			row++)
		{
			deque<float> imgRow;
			for (size_t col = 0;
				col != _img.imageSize().first;
				col++)
			{
				imgRow.push_back(dist(mt));
			}
			this->img.push_back(imgRow);
		}
		this->width = _img.imageSize().first;
		this->height = _img.imageSize().second;
		this->imageName = "随机生成图片";
		return;
	}
	else
	{
		cerr<<"Image(): 未识别的构造类型, 退出"<<endl;
		exit(-1);
	}
}

Image Image::convolution(const Image& _kernel, const bool& _padding = true)
	const
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
			if (this->isDebug)
			{
				cerr<<"\r图像卷积:"<<row+1<<"/"<<result.imageSize().second;
			}
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
				res /= 255;
				result.img[row][col] = res;
			}
		}
		if (this->isDebug)
		{
			cerr<<endl;
		}
	}
	else
	{
		
	}
	result.changeImageName("卷积后的图像");
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

Image Image::operator-(const Image& _rhs)
{
	if (this->imageSize().first == _rhs.imageSize().first &&
		this->imageSize().second == _rhs.imageSize().second)
	{
		Image result = Image(*this, true);
		for (size_t row = 0;
			row != this->imageSize().second;
			row++)
		{
			for (size_t col = 0;
				col != this->imageSize().first;
				col++)
			{
				result.img[row][col] = this->image()[row][col] -
					_rhs.image()[row][col];
			}
		}
		result.changeImageName("差异图");
		return result;
	}
	else
	{
		cerr<<"在进行图片值差异操作时发现图片大小不符, 操作结束"<<endl;
		exit(-1);
	}
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
	fstream imageFile(_filename, ios::out|ios::binary);
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
	return true;
}

void Image::minMaxNormalization(const contentType& _minValue = 0.0,
	const contentType& _maxValue = 1.0)
{
	vector<contentType> valueList;
	for (const auto& row: this->image())
	{
		auto ans = minmax_element(row.begin(), row.end());
		valueList.push_back(*ans.first);
		valueList.push_back(*ans.second);
	}
	auto ans = minmax_element(valueList.begin(), valueList.end());
	this->minMaxNormalization(_minValue, _maxValue,
		*ans.first, *ans.second);
	return;
}

void Image::minMaxNormalization(contentType _minValueTo,
	contentType _maxValueTo,
	contentType _minValueFrom,
	contentType _maxValueFrom)
{
	if (_maxValueTo < _minValueTo)
	{
		contentType tmp = _minValueTo;
		_minValueTo = _maxValueTo;
		_maxValueTo = tmp;
	}
	if (_maxValueFrom < _minValueFrom)
	{
		contentType tmp = _minValueFrom;
		_minValueFrom = _maxValueFrom;
		_maxValueFrom = tmp;
	}
	contentType normTimes = (_maxValueTo - _minValueTo)/
		(_maxValueFrom - _minValueFrom);
	for (auto& row: this->img)
	{
		for_each(row.begin(), row.end(), [&](contentType& _ele)
		{
			_ele -= _minValueFrom;
			_ele *= normTimes;
			_ele += _minValueTo;
			if (_ele < _minValueTo)
			{
				_ele = _minValueTo;
			}
			else if(_ele > _maxValueTo)
			{
				_ele = _maxValueTo;
			}
		});
	}
	return;
}

Image::contentType Image::trainingKernel(const Image& _rawImage,
	const Image& _diffMap, const contentType& _learningRate = 0.0001)
{
	pair<float, pair<size_t, size_t>> diffPosition = {0, {-1, -1}};
	for (size_t row = 0;
		row != _diffMap.imageSize().second;
		row++)
	{
		for (size_t col = 0;
			col != _diffMap.imageSize().first;
			col++)
		{
			if (abs(_diffMap.image()[row][col]) > diffPosition.first)
			{
				diffPosition = {abs(_diffMap.image()[row][col]), {row, col}};
			}
		}
	}
	if (diffPosition.first == 0)
	{
		if (this->isDebug)
		{
			cerr<<"无差异"<<endl;
		}
		return diffPosition.first;
	}
	if (this->isDebug)
	{
		cout<<"找到差异: "<<diffPosition.first<<". 位置为("<<
			diffPosition.second.second+1<<","<<diffPosition.second.first+1<<
			"): 内部坐标("<<diffPosition.second.first<<","<<
			diffPosition.second.second<<");"<<endl;
	}
	int paddingLength = (this->imageSize().first-1)/2;
	Image paddedImage(_rawImage.imageSize(), paddingLength);
	paddedImage.paste(_rawImage, paddingLength, paddingLength);
	for (size_t kernelRow = 0;
		kernelRow != this->imageSize().second;
		kernelRow++)
	{
		for (size_t kernelCol = 0;
			kernelCol != this->imageSize().first;
			kernelCol++)
		{
			this->img[kernelRow][kernelCol] += _learningRate *
				(this->img[kernelRow][kernelCol]) *
				paddedImage.image()[
					diffPosition.second.second + kernelRow
				][
					diffPosition.second.first + kernelCol
				];
		}
	}
	return abs(diffPosition.first);
}

void Image::dump() const
{
	this->dump("normal");
	return;
}

void Image::dump(const string& _dumpMode) const
{
	if (_dumpMode == "csv")
	{
		for (size_t row = 0;
			row != this->imageSize().second;
			row++)
		{
			for (size_t col = 0;
				col != this->imageSize().first;
				col++)
			{
				cout<<this->image()[row][col];
				if (col != this->imageSize().first-1)
				{
					cout<<",";
				}
			}
			cout<<endl;
		}
		return;
	}
	cout<<"图片: "<<this->imageName<<endl;
	cout<<"尺寸: "<<this->imageSize().second<<"*"
		<<this->imageSize().first<<endl;
	cout<<"[";
	for (size_t row = 0;
		row != this->imageSize().second;
		row++)
	{
		cout<<"\t[";
		if (this->imageSize().first > 6 && _dumpMode != "all")
		{
			const deque<float>& col = this->image()[row];
			const size_t colSize = this->imageSize().first;
			cout<<col[0]<<"\t"<<col[1]<<"\t"<<col[2]<<"\t...\t"<<
				col[colSize-3]<<"\t"<<col[colSize-2]<<"\t"<<col[colSize-1]<<
				"]("<<colSize<<")"<<endl;
		}
		else
		{
			for (size_t col = 0;
				col != this->imageSize().first;
				col++)
			{
				cout<<this->image()[row][col];
				if (col != this->imageSize().first-1)
				{
					cout<<"\t";
				}
			}
			cout<<"]"<<endl;
			if (this->imageSize().second > 8)
			{
				cout<<endl;
			}
		}
	}
	cout<<"]("<<this->imageSize().second<<")";
	cout<<endl;
	return;
}

void Image::changeImageName(const string& _imageName)
{
	this->imageName = _imageName;
	return;
}

bool Image::openImage(const string& _filename)
{
	fstream imageFile(_filename, ios::in|ios::binary);
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
	this->imageName = "";
	this->isDebug = false;
	return;
}

int main(int argc, char* argv[])
{
	Image img("dbg/image.ppm");
	img.minMaxNormalization(0, 1, 0, 255);
	Image kernel("dbg/kernel.ppm");
	kernel.minMaxNormalization(0, 1, 0, 255);
	Image randomKernel(kernel, "random");
	randomKernel.minMaxNormalization(0, 1, 0, 255);
	Image featureMap = img.convolution(kernel);
	for (size_t times=0; times!=1; times++)
	{
		Image randomFeatureMap = img.convolution(randomKernel);
		Image diff = featureMap - randomFeatureMap;
		float delta = randomKernel.trainingKernel(img, diff, 0.00001);
		cout<<"\r"<<times+1<<":"<<delta<<endl;
	}
	randomKernel.minMaxNormalization(0, 255);
	randomKernel.dump("all");
	randomKernel.save("dbg/ans.ppm");
	return 0;
}