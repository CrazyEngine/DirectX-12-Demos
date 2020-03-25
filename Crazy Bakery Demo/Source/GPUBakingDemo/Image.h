#pragma once

#include <vector_types.h>
//#include "Utility.h"
template<class T>
inline T Clamp(const T &_val, const T &_low, const T &_high)
{
	if (_val < _low)
		return _low;

	if (_val > _high)
		return _high;

	return _val;
}

//namespace CrazyBakery
//{
	class ImageSample
	{
	public:

		int x, y;
		float4 c;
	};

	class PixelG8u
	{
	public:

		PixelG8u();

	public:

		PixelG8u& operator = (const PixelG8u &_p);
		PixelG8u& operator = (const float &_p);
		PixelG8u& operator = (const float3 &_p);
		PixelG8u& operator = (const float4 &_p);
		PixelG8u& operator += (const PixelG8u &_p);
		PixelG8u& operator += (const float &_p);
		PixelG8u& operator += (const float3 &_p);
		PixelG8u& operator += (const float4 &_p);
		float3 GetFloat() const;
		void GetValue(float3 &p_) const;
		void GetValue(float4 &p_) const;
		void Clear();

	public:

		unsigned char greyscale;
	};

	class PixelGf
	{
	public:

		PixelGf();

	public:

		PixelGf& operator = (const PixelGf &_p);
		PixelGf& operator = (const float &_p);
		PixelGf& operator = (const float3 &_p);
		PixelGf& operator = (const float4 &_p);
		PixelGf& operator += (const PixelGf &_p);
		PixelGf& operator += (const float &_p);
		PixelGf& operator += (const float3 &_p);
		PixelGf& operator += (const float4 &_p);
		float3 GetFloat() const;
		void GetValue(float3 &p_) const;
		void GetValue(float4 &p_) const;
		void Clear();

	public:

		float greyscale;
	};

	class PixelRGBf
	{
	public:

		PixelRGBf();

	public:

		PixelRGBf& operator = (const PixelRGBf &_p);
		PixelRGBf& operator = (const float &_p);
		PixelRGBf& operator = (const float3 &_p);
		PixelRGBf& operator = (const float4 &_p);
		PixelRGBf& operator += (const PixelRGBf &p);
		PixelRGBf& operator += (const float &_p);
		PixelRGBf& operator += (const float3 &_p);
		PixelRGBf& operator += (const float4 &_p);
		float3 GetFloat() const;
		void GetValue(float3 &p_) const;
		void GetValue(float4 &p_) const;
		void Clear();

	public:

		float r, g, b;
	};

	class PixelRGB8u
	{
	public:

		PixelRGB8u();

	public:

		PixelRGB8u& operator = (const PixelRGB8u &_p);
		PixelRGB8u& operator = (const float &_p);
		PixelRGB8u& operator = (const float3 &_p);
		PixelRGB8u& operator = (const float4 &_p);
		PixelRGB8u& operator += (const PixelRGB8u &p);
		PixelRGB8u& operator += (const float &_p);
		PixelRGB8u& operator += (const float3 &_p);
		PixelRGB8u& operator += (const float4 &_p);
		float3 GetFloat() const;
		void GetValue(float3 &p_) const;
		void GetValue(float4 &p_) const;
		void Clear();

	public:

		unsigned char r, g, b;
	};

	class PixelRGBWf
	{
	public:

		PixelRGBWf();

	public:

		PixelRGBWf& operator = (const PixelRGBWf &_p);
		PixelRGBWf& operator = (const float &_p);
		PixelRGBWf& operator = (const float3 &_p);
		PixelRGBWf& operator = (const float4 &_p);
		PixelRGBWf& operator += (const PixelRGBWf &_p);
		PixelRGBWf& operator += (const float &_p);
		PixelRGBWf& operator += (const float3 &_p);
		PixelRGBWf& operator += (const float4 &_p);
		float3 GetFloat() const;
		void GetValue(float3 &p_) const;
		void GetValue(float4 &p_) const;
		void Clear();

	public:

		float r, g, b, w;
	};

	class Image
	{
	public:

		Image();
		Image(int w, int h);
		virtual ~Image();

	public:

		int GetWidth() const;
		int GetHeight() const;
		int GetSize() const;
		void CopyFrom(const Image &_img);
		void CopyTo(Image &img_) const;

	public:

		virtual float3 GetPixel(int _x, int _y) const = 0;
		virtual float3 GetPixel(float _u, float _v) const = 0;
		virtual float3 GetPixel(const float2 &_uv) const = 0;
		virtual void  GetPixel(float3 &p_, int _x, int _y) const = 0;
		virtual void  GetPixel(float4 &p_, int _x, int _y) const = 0;
		virtual void  GetPixel(float3 &p_, float _u, float _v) const = 0;
		virtual void  GetPixel(float4 &p_, float _u, float _v) const = 0;
		virtual void  GetPixel(float3 &p_, const float2 &_uv) const = 0;
		virtual void  GetPixel(float4 &p_, const float2 &_uv) const = 0;
		virtual void SetPixel(int _x, int _y, float _p) = 0;
		virtual void SetPixel(const int _x, const int _y, const float3 &_p) = 0;
		virtual void SetPixel(const int _x, const int _y, const float4 &_p) = 0;
		virtual void SetPixel(const float3 &_p) = 0;
		virtual void SetPixel(const float4 &_p) = 0;
		virtual void SetPixel(float _p) = 0;
		virtual void SetPixel(const Image &_img) = 0;
		virtual void SetPixel(const Image &_img, const int _px, const int _py, const int _lx, const int _ly) = 0;
		virtual void AddPixel(const int _x, const int _y, const float3 &_p) = 0;
		virtual void AddPixel(const int _x, const int _y, const float4 &_p) = 0;
		virtual void AddPixel(const float3 &_p) = 0;
		virtual void AddPixel(const float4 &_p) = 0;
		virtual void AddPixel(float _p) = 0;
		virtual void AddPixel(const Image &_p) = 0;
		virtual void Resize(int _w, int _h) = 0;
		virtual void Clear() = 0;
		virtual Image* Duplicate(bool _isCopy = true) const = 0;
		virtual void  Duplicate(float3 *image_, const int w, const int h) const = 0;
		virtual int GetPixelSize() const = 0;

	protected:

		int m_width, m_height;
	};

	template<class PixelType>
	class ImageT : public Image
	{
	public:

		ImageT() : Image()
		{
			m_pixel = 0;
		}
		ImageT(int _w, int _h)
			: Image(_w, _h)
		{
			m_pixel = new PixelType[m_width * m_height];
		}
		virtual ~ImageT()
		{
			if (m_pixel)
				delete[] m_pixel;
		}

	public:

		virtual float3 GetPixel(int _x, int _y) const
		{
			return m_pixel[_y * m_width + _x].GetFloat();
		}
		virtual float3 GetPixel(float _u, float _v) const
		{
			const int x = Clamp(int(_u * m_width), 0, m_width - 1);
			const int y = Clamp(int(_v * m_height), 0, m_height - 1);

			return m_pixel[y * m_width + x].GetFloat();
		}
		virtual float3 GetPixel(const float2 &_uv) const
		{
			return GetPixel(_uv.x, _uv.y);
		}
		virtual void GetPixel(float3 &p_, int _x, int _y) const
		{
			m_pixel[_y * m_width + _x].GetValue(p_);
		}
		virtual void GetPixel(float4 &p_, int _x, int _y) const
		{
			m_pixel[_y * m_width + _x].GetValue(p_);
		}
		virtual void GetPixel(float3 &p_, float _u, float _v) const
		{
			const int x = Clamp(int(_u * m_width), 0, m_width - 1);
			const int y = Clamp(int(_v * m_height), 0, m_height - 1);
			m_pixel[y * m_width + x].GetValue(p_);
		}
		virtual void GetPixel(float4 &p_, float _u, float _v) const
		{
			const int x = Clamp(int(_u * m_width), 0, m_width - 1);
			const int y = Clamp(int(_v * m_height), 0, m_height - 1);
			m_pixel[y * m_width + x].GetValue(p_);
		}
		virtual void GetPixel(float3 &p_, const float2 &_uv) const
		{
			GetPixel(p_, _uv.x, _uv.y);
		}
		virtual void GetPixel(float4 &p_, const float2 &_uv) const
		{
			GetPixel(p_, _uv.x, _uv.y);
		}
		virtual void SetPixel(int _x, int _y, float _p)
		{
			m_pixel[_y * m_width + _x] = _p;
		}
		virtual void SetPixel(const int _x, const int _y, const float _p[3])
		{
			m_pixel[_y * m_width + _x] = float3{ _p[0], _p[1], _p[2] };
		}
		virtual void SetPixel(const int _x, const int _y, const float3 &_p)
		{
			m_pixel[_y * m_width + _x] = _p;
		}
		virtual void SetPixel(const int _x, const int _y, const float4 &_p)
		{
			m_pixel[_y * m_width + _x] = _p;
		}
		virtual void SetPixel(const float3 &_p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] = _p;
		}
		virtual void SetPixel(const float4 &_p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] = _p;
		}
		virtual void SetPixel(float _p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] = _p;
		}
		virtual void SetPixel(const Image &_img)
		{
			if (m_width != _img.GetWidth() || m_height != _img.GetHeight())
				return;

			for (int y = 0; y < m_height; y++)
				for (int x = 0; x < m_width; x++)
				{
					float4 p;
					_img.GetPixel(p, x, y);

					SetPixel(x, y, p);
				}
		}
		virtual void SetPixel(const Image &_img, const int _px, const int _py, const int _lx, const int _ly)
		{
			if (m_width != _img.GetWidth() || m_height != _img.GetHeight())
				return;

			for (int y = _py; y < _py + _ly; y++)
				for (int x = _px; x < _px + _lx; x++)
				{
					float4 p;
					_img.GetPixel(p, x, y);

					SetPixel(x, y, p);
				}
		}
		virtual void AddPixel(const int _x, const int _y, const float3 &_p)
		{
			m_pixel[_y * m_width + _x] += _p;
		}
		virtual void AddPixel(const int _x, const int _y, const float4 &_p)
		{
			m_pixel[_y * m_width + _x] += _p;
		}
		virtual void AddPixel(const float3 &_p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] += _p;
		}
		virtual void AddPixel(const float4 &_p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] += _p;
		}
		virtual void AddPixel(float _p)
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i] += _p;
		}
		virtual void AddPixel(const Image &_img)
		{
			if (m_width != _img.GetWidth() || m_height != _img.GetHeight())
				return;

			for (int y = 0; y < m_height; y++)
				for (int x = 0; x < m_width; x++)
				{
					float4 p;
					_img.GetPixel(p, x, y);
					AddPixel(x, y, p);
				}
		}
		virtual void Resize(int _w, int _h)
		{
			if (_w != m_width || _h != m_height)
			{
				if (m_pixel)
					delete[] m_pixel;

				m_width = _w;
				m_height = _h;
				m_pixel = new PixelType[m_width * m_height];
			}

			Clear();
		}
		virtual void Clear()
		{
			for (int j = 0, row = 0; j < m_height; j++, row += m_width)
				for (int i = 0; i < m_width; i++)
					m_pixel[row + i].Clear();
		}

		virtual Image* Duplicate(bool _isCopy = true) const
		{
			Image *img = new ImageT<PixelType>(m_width, m_height);

			if (_isCopy)
				img->SetPixel(*this);

			return img;
		}
		virtual void Duplicate(float3 *image_, const int _w, const int _h) const
		{
			const int width = min(m_width, _w);
			const int height = min(m_height, _h);
			for (int i = 0, k = 0; i < height; i++)
				for (int j = 0; j < width; j++, k++)
					GetPixel(image_[k], j, i);
		}
		virtual int GetPixelSize() const
		{
			return sizeof(PixelType);
		}

	private:

		PixelType *m_pixel;
	};

	typedef ImageT<PixelGf> ImageGf;
	typedef ImageT<PixelG8u> ImageG8u;
	typedef ImageT<PixelRGBf> ImageRGBf;
	typedef ImageT<PixelRGB8u> ImageRGB8u;
	typedef ImageT<PixelRGBWf> ImageRGBWf;
//}
