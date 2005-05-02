// PAS 17/6/04 (revised 16/2/05)

#ifndef __CVD_DEINTERLACEBUFFER_H
#define __CVD_DEINTERLACEBUFFER_H

#include <cvd/localvideobuffer.h>
#include <cvd/image_convert.h>
#include <cvd/colourspace_convert.h>
#include <cvd/colourspace_frame.h>

namespace CVD
{
/////////////////
// DEINTERLACE BUFFER
//

/// A decorator class which wraps a VideoBuffer to perfrom colourspace
/// conversion on the incoming data. In general, the kernel video buffers
/// such as dvbuffer and v4l1buffer give access to what the device can provide
/// natively. However, the native formats may not be what is required. This 
/// buffer transparently converts incoming frames to the required type.
///
/// Not every possible conversion is available natively through the library,
/// some conversions have to be performed in several steps. For instance, to 
/// convert yuv411 to Rgb<float>, the conversion must go via Rgb<byte> since
/// the conversions from yuv411 are limited.



/// Provides frames of type CVD::ColourspaceFrame.
///
/// This class throws only generic VideoBuffer exceptions, but the underlying
/// videobuffer may throw.
/// @param From  The pixel type of the original VideoBuffer
/// @param To  The pixel type to convert in to.
/// @ingroup gVideoBuffer
template <class To, class From> class ColourspaceBuffer : public CVD::LocalVideoBuffer<To>
{
	public:
		/// Construct a ColourspaceBuffer by wrapping it around another VideoBuffer
		/// @param buf The buffer that will provide the raw frames
   		ColourspaceBuffer(CVD::VideoBuffer<From>& buf)
		:m_vidbuf(buf),m_size(buf.size())
		{
		}
 
		/// The size of the VideoFrames returns by this buffer.
		ImageRef size()
		{
			return m_size;	
		}
		
		virtual bool frame_pending()
		{	
			return m_vidbuf.frame_pending();
		}
			
		virtual void seek_to(double t)
		{
			return m_vidbuf.seek_to(t);
		}
			
		virtual double frame_rate()
	  	{
			return m_vidbuf.frame_rate();
		}

		virtual CVD::ColourspaceFrame<To>* get_frame()
		{
			VideoFrame<From>* fr = m_vidbuf.get_frame();
			Image<To> cv = convert_image<To>(*fr);

			ColourspaceFrame<To>* ret = new ColourspaceFrame<To>(fr->timestamp(), cv);

			m_vidbuf.put_frame(fr);

			return ret;
		}

		virtual void put_frame(CVD::VideoFrame<To>* f)
		{
			//Check that the type is correct...
			ColourspaceFrame<To>* csf = dynamic_cast<ColourspaceFrame<To>*>(f);

			if(csf == NULL)
				throw CVD::Exceptions::VideoBuffer::BadPutFrame();
			else 
				delete csf;
		}

   private:
		CVD::VideoBuffer<From>& m_vidbuf;
		ImageRef m_size;
};


/// This is just like ColourspaceBuffer, except it deleted the videobuffer on destruction
template <class To, class From> class ColourspaceBuffer_managed : public ColourspaceBuffer<To, From>
{
	public:
		/// Construct a ColourspaceBuffer by wrapping it around another VideoBuffer
		/// @param buf The buffer that will provide the raw frames
   		ColourspaceBuffer_managed(CVD::VideoBuffer<From>* buf)
		:ColourspaceBuffer<To,From>(*buf),vb(buf)
		{
		}

		~ColourspaceBuffer_managed()
		{
			delete  vb;
		}

	private:
		VideoBuffer<From>* vb;

};

} 
#endif
