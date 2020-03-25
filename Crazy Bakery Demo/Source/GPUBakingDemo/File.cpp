#include "stdafx.h"
#include "File.h"
#include "FreeImage.h"
#include <algorithm>

using namespace std;

//namespace CrazyBakery
//{
	class FreeImageInitializer
	{
	public:
		FreeImageInitializer() { FreeImage_Initialise(); }
		~FreeImageInitializer() { FreeImage_DeInitialise(); }
	} g_initializer;

	// Translate memory image format to file image format.
	inline FREE_IMAGE_TYPE GetImageType(File::PixelChannel _c, File::PixelFormat _f)
	{
		if (_c == File::RGB && _f == File::f32)
			return FIT_RGBF;

		if (_c == File::Greyscale && _f == File::f32)
			return FIT_FLOAT;

		if (_c == File::RGB && _f == File::u8)
			return FIT_BITMAP;

		if (_c == File::RGBA && _f == File::u8)
			return FIT_BITMAP;

		if (_c == File::Greyscale && _f == File::u8)
			return FIT_BITMAP;

		return FIT_UNKNOWN;
	}

	inline void GetImageType(
		File::PixelChannel &c_,
		File::PixelFormat &f_,
		unsigned int _bpp,
		FREE_IMAGE_TYPE _t)
	{
		if (_t == FIT_RGBF && _bpp == 96)
		{
			c_ = File::RGB;
			f_ = File::f32;
			return;
		}

		if (_t == FIT_FLOAT && _bpp == 32)
		{
			c_ = File::Greyscale;
			f_ = File::f32;
			return;
		}

		if (_t == FIT_BITMAP && _bpp == 8)
		{
			c_ = File::Greyscale;
			f_ = File::u8;
			return;
		}

		if (_t == FIT_BITMAP && _bpp == 24)
		{
			c_ = File::RGB;
			f_ = File::u8;
			return;
		}

		if (_t == FIT_BITMAP && _bpp == 32)
		{
			c_ = File::RGBA;
			f_ = File::u8;
			return;
		}
	}

	// Determine bit per pixel from memory image format.
	inline int GetBPP(File::PixelChannel _c, File::PixelFormat _f)
	{
		if (_c == File::RGB && _f == File::f32)
			return 96;

		if (_c == File::Greyscale && _f == File::f32)
			return 32;

		if (_c == File::Greyscale && _f == File::u8)
			return 8;

		if (_c == File::RGB && _f == File::u8)
			return 24;

		if (_c == File::RGBA && _f == File::u8)
			return 32;

		return 0;
	}

	// Convert a line of pixels from memory image to file image (RGBF version).
	void ConvertLineRGBF(FIRGBF *line_, int _y, const Image &_image, float _gamma)
	{
		for (int x = 0; x < _image.GetWidth(); x++)
		{
			const float3 color = _image.GetPixel(x, _y);
			line_[x].red = pow(color.x, 1.f / _gamma);
			line_[x].green = pow(color.y, 1.f / _gamma);
			line_[x].blue = pow(color.z, 1.f / _gamma);
		}
	}

	// Convert a line of pixels from file image to memory image (RGBF version).
	void ConvertLineRGBF(Image &image_, int _y, FIRGBF *_line)
	{
		for (int x = 0; x < image_.GetWidth(); x++)
		{
			float3 color{ _line[x].red, _line[x].green, _line[x].blue };
			image_.SetPixel(x, _y, color);
		}
	}

	// Convert a line of pixels from memory image to file image (float version).
	void ConvertLineFloat(float *line_, int _y, const Image &_image, float _gamma)
	{
		for (int x = 0; x < _image.GetWidth(); x++)
		{
			const float3 color = _image.GetPixel(x, _y);
			line_[x] = pow(color.x, 1.f / _gamma);
		}
	}

	// Convert a line of pixels from file image to memory image (float version).
	void ConvertLineFloat(Image &image_, int _y, float *_line)
	{
		for (int x = 0; x < image_.GetWidth(); x++)
		{
			const float3 color{ _line[x], _line[x], _line[x] };
			image_.SetPixel(x, _y, color);
		}
	}

	// Convert a line of pixels from memory image to file image (RGB8u version).
	void ConvertLineBM24(BYTE *line_, int _y, const Image &_image, float _gamma)
	{
		for (int x = 0; x < _image.GetWidth(); x++)
		{
			const float3 color = _image.GetPixel(x, _y);
			line_[FI_RGBA_RED] = (BYTE)max(0.f, min(pow(color.x, 1.f / _gamma) * 255.f, 255.f));
			line_[FI_RGBA_GREEN] = (BYTE)max(0.f, min(pow(color.y, 1.f / _gamma) * 255.f, 255.f));
			line_[FI_RGBA_BLUE] = (BYTE)max(0.f, min(pow(color.z, 1.f / _gamma) * 255.f, 255.f));
			line_ += 3;
		}
	}

	// Convert a line of pixels from file image to memory image (RGB8u version).
	void ConvertLineBM24(Image &image_, int _y, BYTE *_line)
	{
		for (int x = 0; x < image_.GetWidth(); x++)
		{
			float3 color;
			color.x = _line[FI_RGBA_RED] / 255.f;
			color.y = _line[FI_RGBA_GREEN] / 255.f;
			color.z = _line[FI_RGBA_BLUE] / 255.f;
			image_.SetPixel(x, _y, color);
			_line += 3;
		}
	}

	// Convert a line of pixels from memory image to file image (Greyscale8u version).
	void ConvertLineBM8(BYTE *line_, int _y, const Image &_image, float _gamma)
	{
		for (int x = 0; x < _image.GetWidth(); x++)
		{
			const float3 color = _image.GetPixel(x, _y);
			*(line_++) = (BYTE)max(0.f, min(pow(color.x, 1.f / _gamma) * 255.f, 255.f));
		}
	}

	// Convert a line of pixels from file image to memory image  (Greyscale8u version).
	void ConvertLineBM8(Image &image_, int _y, BYTE *_line)
	{
		for (int x = 0; x < image_.GetWidth(); x++)
		{
			float3 color;
			color.x = color.y = color.z = *(_line++) / 255.f;
			image_.SetPixel(x, _y, color);
		}
	}

	// Convert memory image to file image.
	void ConvertImage(
		FIBITMAP *fi_,
		const Image &_image,
		const File::PixelChannel _c,
		const File::PixelFormat _f,
		const float _gamma)
	{
		const int height = _image.GetHeight();

		// Convert image composed of float RGB pixels.
		if (_c == File::RGB && _f == File::f32)
		{
			for (int y = 0; y < height; y++)
			{
				BYTE *line = FreeImage_GetScanLine(fi_, y);
				ConvertLineRGBF((FIRGBF*)line, y, _image, _gamma);
			}
			return;
		}

		// Convert image composed of float greyscale pixels.
		if (_c == File::Greyscale && _f == File::f32)
		{
			for (int y = 0; y < height; y++)
			{
				BYTE *line = FreeImage_GetScanLine(fi_, y);
				ConvertLineFloat((float*)line, y, _image, _gamma);
			}
			return;
		}

		// Convert image composed of 8 bit integer RGB pixels.
		if (_c == File::RGB && _f == File::u8)
		{
			for (int y = 0; y < height; y++)
			{
				BYTE *line = FreeImage_GetScanLine(fi_, y);
				ConvertLineBM24((BYTE*)line, y, _image, _gamma);
			}
			return;
		}

		// Convert image composed of 8 bit integer greyscale pixels.
		if (_c == File::Greyscale && _f == File::u8)
		{
			for (int y = 0; y < height; y++)
			{
				BYTE *line = FreeImage_GetScanLine(fi_, y);
				ConvertLineBM8((BYTE*)line, y, _image, _gamma);
			}
			return;
		}
	}

	void ConvertBM8(Image &image_, FIBITMAP *_fi)
	{
		const unsigned char *bits = (unsigned char*)FreeImage_GetBits(_fi);
		const unsigned int pitch = FreeImage_GetPitch(_fi) / (sizeof(unsigned char));
		for (int y = 0; y < image_.GetHeight(); y++)
		{
			const unsigned char *pixel = (unsigned char*)bits;
			for (int x = 0; x < image_.GetWidth(); x++)
			{
				image_.SetPixel(x, y, *pixel / 255.f);
				pixel++;
			}

			bits += pitch;
		}
	}

	void ConvertBM24(Image &image_, FIBITMAP *_fi)
	{
		const unsigned char *bits = (unsigned char*)FreeImage_GetBits(_fi);
		const unsigned int pitch = FreeImage_GetPitch(_fi) / sizeof(unsigned char);
		for (int y = 0; y < image_.GetHeight(); y++)
		{
			const unsigned char *pixel = (unsigned char*)bits;
			for (int x = 0; x < image_.GetWidth(); x++)
			{
				const float3 c{ pixel[FI_RGBA_RED] / 255.f, pixel[FI_RGBA_GREEN] / 255.f, pixel[FI_RGBA_BLUE] / 255.f };
				image_.SetPixel(x, y, c);
				pixel += 3;
			}

			bits += pitch;
		}
	}

	void ConvertBM32(Image &image_, FIBITMAP *_fi)
	{
		const unsigned char *bits = (unsigned char*)FreeImage_GetBits(_fi);
		const unsigned int pitch = FreeImage_GetPitch(_fi) / sizeof(unsigned char);
		for (int y = 0; y < image_.GetHeight(); y++)
		{
			const unsigned char *pixel = (unsigned char*)bits;
			for (int x = 0; x < image_.GetWidth(); x++)
			{
				const float3 c{ pixel[FI_RGBA_RED] / 255.f, pixel[FI_RGBA_GREEN] / 255.f, pixel[FI_RGBA_BLUE] / 255.f };
				image_.SetPixel(x, y, c);
				pixel += 4;
			}

			bits += pitch;
		}
	}

	void ConvertFloat(Image &image_, FIBITMAP *_fi)
	{
		const float *bits = (float*)FreeImage_GetBits(_fi);
		const unsigned int pitch = FreeImage_GetPitch(_fi) / sizeof(float);
		for (int y = 0; y < image_.GetHeight(); y++)
		{
			const float *pixel = (float*)bits;
			for (int x = 0; x < image_.GetWidth(); x++)
			{
				image_.SetPixel(x, y, *pixel);
				pixel++;
			}

			bits += pitch;
		}
	}

	void ConvertRGBF(Image &image_, FIBITMAP *_fi)
	{
		const FIRGBF *bits = (FIRGBF*)FreeImage_GetBits(_fi);
		const unsigned int pitch = FreeImage_GetPitch(_fi) / sizeof(tagFIRGBF);
		for (int y = 0; y < image_.GetHeight(); y++)
		{
			const FIRGBF *pixel = (FIRGBF*)bits;
			for (int x = 0; x < image_.GetWidth(); x++)
			{
				const float3 c{ pixel->blue, pixel->green, pixel->red };
				image_.SetPixel(x, y, c);
				pixel++;
			}

			bits += pitch;
		}
	}

	// Convert file image to memory image.
	void ConvertImage(
		Image &image_,
		FIBITMAP *_fi,
		File::PixelChannel _c,
		File::PixelFormat _f)
	{
		const int height = image_.GetHeight();
		const unsigned char *line = FreeImage_GetScanLine(_fi, height);

		// Convert image composed of float RGB pixels.
		if (_c == File::RGB && _f == File::f32)
		{
			ConvertRGBF(image_, _fi);
			return;
		}

		// Convert image composed of float greyscale pixels.
		if (_c == File::Greyscale && _f == File::f32)
		{
			ConvertFloat(image_, _fi);
			return;
		}

		// Convert image composed of 8 bit integer greyscale pixels.
		if (_c == File::Greyscale && _f == File::u8)
		{
			ConvertBM8(image_, _fi);
			return;
		}

		// Convert image composed of 8 bit integer RGB pixels.
		if (_c == File::RGB && _f == File::u8)
		{
			ConvertBM24(image_, _fi);
			return;
		}

		// Convert image composed of 8 bit integer RGB pixels.
		if (_c == File::RGBA && _f == File::u8)
		{
			ConvertBM32(image_, _fi);
			return;
		}
	}

	File::File()
	{
		m_channel = UnknownChannel;
		m_format = UnknownFormat;
		m_gamma = 1.f;
		m_newSize = int2{ 0, 0 };
	}

	File::~File()
	{

	}

	void File::Save(const Image &_image) const
	{
		const int width = _image.GetWidth();
		const int height = _image.GetHeight();
		const FREE_IMAGE_TYPE fit = GetImageType(m_channel, m_format);
		const int bpp = GetBPP(m_channel, m_format);

		// Allocate temporary storage for image file.
		FIBITMAP *fi = FreeImage_AllocateT(fit, width, height, bpp);

		// Convert pixel from runtime format to image file format.
		ConvertImage(fi, _image, m_channel, m_format, m_gamma);

		// Flush data from temporary storage to disk.
		const FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(m_path.c_str());

		FreeImage_SaveU(fif, fi, m_path.c_str());

		FreeImage_Unload(fi);
	}

	void AdjustChannel(Image &_image_)
	{
		for (int y = 0; y < _image_.GetHeight(); y++)
		{
			for (int x = 0; x < _image_.GetWidth(); x++)
			{
				float3 c = _image_.GetPixel(x, y);
				swap(c.x, c.z);
				_image_.SetPixel(x, y, c);
			}
		}
	}

	void FlipVertical(Image &_image_)
	{
		for (int y1 = 0, y2 = _image_.GetHeight() - 1; y1 < y2; y1++, y2--)
		{
			for (int x = 0; x < _image_.GetWidth(); x++)
			{
				const float3 c1 = _image_.GetPixel(x, y1);
				const float3 c2 = _image_.GetPixel(x, y2);
				_image_.SetPixel(x, y1, c2);
				_image_.SetPixel(x, y2, c1);
			}
		}
	}

	void File::Load(Image &image_)
	{
		const FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(m_path.c_str());

		if (!FreeImage_FIFSupportsReading(fif))
			return;

		// Load data from disk to temporary storage.
		FIBITMAP *fi = FreeImage_LoadU(fif, m_path.c_str());
		if (fi == 0)
			return;

		const unsigned int bpp = FreeImage_GetBPP(fi);
		const FREE_IMAGE_TYPE fit = FreeImage_GetImageType(fi);
		GetImageType(m_channel, m_format, bpp, fit);

		int w = FreeImage_GetWidth(fi);
		int h = FreeImage_GetHeight(fi);

		if (m_newSize.x > 0 && m_newSize.y > 0)
		{
			FIBITMAP *old = fi;
			fi = FreeImage_Rescale(fi, m_newSize.x, m_newSize.y);
			FreeImage_Unload(old);
			w = m_newSize.x;
			h = m_newSize.y;
		}

		if (image_.GetWidth() != w || image_.GetHeight() != h)
			image_.Resize(w, h);

		// Convert pixel from image file format to runtime format.
		ConvertImage(image_, fi, m_channel, m_format);
		AdjustChannel(image_);
		//FlipVertical(image);

		FreeImage_Unload(fi);
	}

	int2 File::GetSize() const
	{
		int2 sz{ -1, -1 };
		const FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(m_path.c_str());

		if (!FreeImage_FIFSupportsReading(fif))
			return sz;

		// Load data from disk to temporary storage.
		FIBITMAP *fi = FreeImage_LoadU(fif, m_path.c_str());
		if (fi == 0)
			return sz;

		sz.x = FreeImage_GetWidth(fi);
		sz.y = FreeImage_GetHeight(fi);

		FreeImage_Unload(fi);

		return sz;
	}
//}
