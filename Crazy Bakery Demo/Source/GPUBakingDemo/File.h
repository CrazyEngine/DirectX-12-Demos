#pragma once

#include "vector_types.h"
#include "Image.h"
#include <string>

//namespace CrazyBakery
//{
	class File
	{
	public:

		enum PixelChannel
		{
			UnknownChannel = 0,
			Greyscale,  // 1 channel for greyscale value
			RGB,        // 3 channel for RGB value
			RGBA        // 3 channel for RGB value, 1 for alpha
		};

		enum PixelFormat
		{
			UnknownFormat = 0,
			u8,  // 8-bit unsigned integer for each channel.
			u16, // 16-bit unsigned integer for each channel.
			u32, // 32-bit unsigned integer for each channel.
			f32  // 32-bit float for each channel.
		};

	public:

		File();
		virtual ~File();

	public:

		void Save(const Image &_image) const;
		void Load(Image &image_);
		int2 GetSize() const;

	public:

		PixelChannel m_channel; // Pixel channel type of the file.
		PixelFormat m_format; // Pixel channel format of the file.
		std::wstring m_path; // System path of the file.
		float m_gamma; // Value for gamma correction.
		int2 m_newSize; // The new size of image when loaded to memory.
	};
//}
