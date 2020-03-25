#include "stdafx.h"
#include "Image.h"

//namespace CrazyBakery
//{
	PixelG8u::PixelG8u()
	{
		greyscale = 0.f;
	}

	PixelG8u& PixelG8u::operator = (const PixelG8u &_p)
	{
		greyscale = _p.greyscale;
		return *this;
	}

	PixelG8u& PixelG8u::operator = (const float &_p)
	{
		greyscale = 255.f * _p;
		return *this;
	}

	PixelG8u& PixelG8u::operator = (const float3 &_p)
	{
		greyscale = 255.f * _p.x;
		return *this;
	}

	PixelG8u& PixelG8u::operator = (const float4 &_p)
	{
		if (_p.w > 0.f)
			greyscale = 255.f * _p.x / _p.w;
		return *this;
	}

	PixelG8u& PixelG8u::operator += (const PixelG8u &_p)
	{
		greyscale += _p.greyscale;
		return *this;
	}

	PixelG8u& PixelG8u::operator += (const float &_p)
	{
		greyscale += 255.f * _p;
		return *this;
	}

	PixelG8u& PixelG8u::operator += (const float3 &_p)
	{
		greyscale += 255.f * _p.x;
		return *this;
	}

	PixelG8u& PixelG8u::operator += (const float4 &_p)
	{
		if (_p.w > 0.f)
			greyscale += 255.f * _p.x / _p.w;
		return *this;
	}

	float3 PixelG8u::GetFloat() const
	{
		return float3{ greyscale / 255.f, greyscale / 255.f, greyscale / 255.f };
	}

	void PixelG8u::GetValue(float3 &p_) const
	{
		p_ = float3{ greyscale / 255.f, greyscale / 255.f, greyscale / 255.f };
	}

	void PixelG8u::GetValue(float4 &p_) const
	{
		p_ = float4{ greyscale / 255.f, greyscale / 255.f, greyscale / 255.f, 1.f };
	}

	void PixelG8u::Clear()
	{
		greyscale = 0;
	}

	PixelGf::PixelGf()
	{
		greyscale = 0.f;
	}

	PixelGf& PixelGf::operator = (const PixelGf &_p)
	{
		greyscale = _p.greyscale;
		return *this;
	}

	PixelGf& PixelGf::operator = (const float &_p)
	{
		greyscale = _p;
		return *this;
	}

	PixelGf& PixelGf::operator = (const float3 &_p)
	{
		greyscale = _p.x;
		return *this;
	}

	PixelGf& PixelGf::operator = (const float4 &_p)
	{
		if (_p.w > 0.f)
			greyscale = _p.x / _p.w;
		return *this;
	}

	PixelGf& PixelGf::operator += (const PixelGf &_p)
	{
		greyscale += _p.greyscale;
		return *this;
	}

	PixelGf& PixelGf::operator += (const float &_p)
	{
		greyscale += _p;
		return *this;
	}

	PixelGf& PixelGf::operator += (const float3 &_p)
	{
		greyscale += _p.x;
		return *this;
	}

	PixelGf& PixelGf::operator += (const float4 &_p)
	{
		if (_p.w > 0.f)
			greyscale += _p.x / _p.w;
		return *this;
	}

	float3 PixelGf::GetFloat() const
	{
		return float3{ greyscale, greyscale, greyscale };
	}

	void PixelGf::GetValue(float3 &p_) const
	{
		p_ = float3{ greyscale, greyscale, greyscale };
	}

	void PixelGf::GetValue(float4 &p_) const
	{
		p_ = float4{ greyscale, greyscale, greyscale, 1.f };
	}

	void PixelGf::Clear()
	{
		greyscale = 0.f;
	}

	PixelRGBf::PixelRGBf()
	{
		r = 0.f;
		g = 0.f;
		b = 0.f;
	}

	PixelRGBf& PixelRGBf::operator = (const PixelRGBf &_p)
	{
		r = _p.r;
		g = _p.g;
		b = _p.b;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator = (const float &_p)
	{
		r = _p;
		g = _p;
		b = _p;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator = (const float3 &_p)
	{
		r = _p.x;
		g = _p.y;
		b = _p.z;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator = (const float4 &_p)
	{
		if (_p.w > 0.f)
		{
			r = _p.x / _p.w;
			g = _p.y / _p.w;
			b = _p.z / _p.w;
		}

		return *this;
	}

	PixelRGBf& PixelRGBf::operator += (const PixelRGBf &_p)
	{
		r += _p.r;
		g += _p.g;
		b += _p.b;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator += (const float &_p)
	{
		r += _p;
		g += _p;
		b += _p;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator += (const float3 &_p)
	{
		r += _p.x;
		g += _p.y;
		b += _p.z;

		return *this;
	}

	PixelRGBf& PixelRGBf::operator += (const float4 &_p)
	{
		if (_p.w > 0.f)
		{
			r += _p.x / _p.w;
			g += _p.y / _p.w;
			b += _p.z / _p.w;
		}
		return *this;
	}

	float3 PixelRGBf::GetFloat() const
	{
		return float3{ r, g, b };
	}

	void PixelRGBf::GetValue(float3 &p_) const
	{
		p_ = float3{ r, g, b };
	}

	void PixelRGBf::GetValue(float4 &p_) const
	{
		p_ = float4{ r, g, b, 1.f };
	}

	void PixelRGBf::Clear()
	{
		r = g = b = 0.f;
	}

	PixelRGB8u::PixelRGB8u()
	{
		r = 0;
		g = 0;
		b = 0;
	}

	PixelRGB8u& PixelRGB8u::operator = (const PixelRGB8u &_p)
	{
		r = _p.r;
		g = _p.g;
		b = _p.b;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator = (const float &_p)
	{
		r = 255.f * _p;
		g = 255.f * _p;
		b = 255.f * _p;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator = (const float3 &_p)
	{
		r = 255.f * _p.x;
		g = 255.f * _p.y;
		b = 255.f * _p.z;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator = (const float4 &_p)
	{
		if (_p.w > 0.f)
		{
			r = 255.f * _p.x / _p.w;
			g = 255.f * _p.y / _p.w;
			b = 255.f * _p.z / _p.w;
		}

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator += (const PixelRGB8u &_p)
	{
		r += _p.r;
		g += _p.g;
		b += _p.b;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator += (const float &_p)
	{
		r += 255.f * _p;
		g += 255.f * _p;
		b += 255.f * _p;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator += (const float3 &_p)
	{
		r += 255.f * _p.x;
		g += 255.f * _p.y;
		b += 255.f * _p.z;

		return *this;
	}

	PixelRGB8u& PixelRGB8u::operator += (const float4 &_p)
	{
		if (_p.w > 0.f)
		{
			r += 255.f * _p.x / _p.w;
			g += 255.f * _p.y / _p.w;
			b += 255.f * _p.z / _p.w;
		}
		return *this;
	}

	float3 PixelRGB8u::GetFloat() const
	{
		return float3{ r / 255.f, g / 255.f, b / 255.f };
	}

	void PixelRGB8u::GetValue(float3 &p_) const
	{
		p_ = float3{ r / 255.f, g / 255.f, b / 255.f };
	}

	void PixelRGB8u::GetValue(float4 &p_) const
	{
		p_ = float4{ r / 255.f, g / 255.f, b / 255.f, 1.f };
	}

	void PixelRGB8u::Clear()
	{
		r = g = b = 0;
	}

	PixelRGBWf::PixelRGBWf()
	{
		r = 0.f;
		g = 0.f;
		b = 0.f;
		w = 0.f;
	}

	PixelRGBWf& PixelRGBWf::operator = (const PixelRGBWf &_p)
	{
		r = _p.r;
		g = _p.g;
		b = _p.b;
		w = _p.w;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator = (const float &_p)
	{
		r = _p;
		g = _p;
		b = _p;
		w = 1.f;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator = (const float3 &_p)
	{
		r = _p.x;
		g = _p.y;
		b = _p.z;
		w = 1.f;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator = (const float4 &_p)
	{
		r = _p.x;
		g = _p.y;
		b = _p.z;
		w = _p.w;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator += (const PixelRGBWf &_p)
	{
		r += _p.r;
		g += _p.g;
		b += _p.b;
		w += _p.w;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator += (const float &_p)
	{
		r += _p;
		g += _p;
		b += _p;
		w += 1.f;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator += (const float3 &_p)
	{
		r += _p.x;
		g += _p.y;
		b += _p.z;
		w += 1.f;

		return *this;
	}

	PixelRGBWf& PixelRGBWf::operator += (const float4 &_p)
	{
		r += _p.x;
		g += _p.y;
		b += _p.z;
		w += _p.w;

		return *this;
	}

	float3 PixelRGBWf::GetFloat() const
	{
		if (w == 0.f)
			return float3{ 0.f, 0.f, 0.f };
		else
			return float3{ r / w, g / w, b / w };
	}

	void PixelRGBWf::GetValue(float3 &p_) const
	{
		if (w == 0.f)
			p_ = float3{ 0.f, 0.f, 0.f };
		else
			p_ = float3{ r / w, g / w, b / w };
	}

	void PixelRGBWf::GetValue(float4 &p_) const
	{
		p_ = float4{ r, g, b, w };
	}

	void PixelRGBWf::Clear()
	{
		r = g = b = w = 0.f;
	}

	Image::Image()
	{
		m_width = 0;
		m_height = 0;
	}

	Image::Image(int _w, int _h)
	{
		m_width = _w;
		m_height = _h;
	}

	Image::~Image()
	{

	}

	int Image::GetWidth() const
	{
		return m_width;
	}

	int Image::GetHeight() const
	{
		return m_height;
	}

	int Image::GetSize() const
	{
		return m_width * m_height;
	}

	void Image::CopyFrom(const Image &_img)
	{
		if (GetWidth() != _img.GetWidth() || GetHeight() != _img.GetHeight())
			return;

		for (int y = 0; y < GetHeight(); y++)
			for (int x = 0; x < GetWidth(); x++)
			{
				const float3 p = _img.GetPixel(x, y);
				SetPixel(x, y, p);
			}
	}

	void Image::CopyTo(Image &img_) const
	{
		if (GetWidth() != img_.GetWidth() || GetHeight() != img_.GetHeight())
			return;

		for (int y = 0; y < GetHeight(); y++)
			for (int x = 0; x < GetWidth(); x++)
			{
				const float3 p = GetPixel(x, y);
				img_.SetPixel(x, y, p);
			}
	}

//}
