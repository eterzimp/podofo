/***************************************************************************
 *   Copyright (C) 2006 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PDF_OUTPUT_DEVICE_H_
#define _PDF_OUTPUT_DEVICE_H_

#include <cstdarg>
#include <ostream>

#include "PdfDefines.h"
#include "PdfLocale.h"

namespace PoDoFo {

class PdfRefCountedBuffer;

/** This class provides an output device which operates 
 *  either on a file or on a buffer in memory.
 *  Additionally it can count the bytes written to the device.
 *
 *  This class is suitable for inheritance to provide output 
 *  devices of your own for PoDoFo.
 *  Just overide the required virtual methods.
 */
class PODOFO_API PdfOutputDevice {
 public:

    /** Construct a new PdfOutputDevice that does not write any data. Only the length
     *  of the data is counted.
     *
     */
    PdfOutputDevice();

    /** Construct a new PdfOutputDevice that writes all data to a file.
     *
     *  \param pszFilename path to a file that will be opened and all data
     *                     is written to this file.
     */
    PdfOutputDevice( const char* pszFilename );

#ifdef _WIN32
    /** Construct a new PdfOutputDevice that writes all data to a file.
     *
     *  \param pszFilename path to a file that will be opened and all data
     *                     is written to this file.
     *
     *  This is an overloaded member function to allow working
     *  with unicode characters. On Unix systes you can also path
     *  UTF-8 to the const char* overload.
     */
    PdfOutputDevice( const wchar_t* pszFilename );
#endif // _WIN32

    /** Construct a new PdfOutputDevice that writes all data to a memory buffer.
     *  The buffer will not be owned by this object and has to be allocated before.
     *
     *  \param pBuffer a buffer in memory
     *  \param lLen the length of the buffer in memory
     */
    PdfOutputDevice( char* pBuffer, size_t lLen );

    /** Construct a new PdfOutputDevice that writes all data to a std::ostream.
     *
     * PdfOutputDevice takes ownership of the stream and will destroy it when writing
     * is complete.
     * TODO: Provide option to leave stream intact and restore its locale at end of
     *       writing.
     *
     *  \param pOutStream write to this std::ostream
     */
    PdfOutputDevice( const std::ostream* pOutStream );

    /** Construct a new PdfOutputDevice that writes all data to a PdfRefCountedBuffer.
     *  This output device has the advantage that the PdfRefCountedBuffer will resize itself
     *  if more memory is needed to hold all data.
     *
     *  \param pOutBuffer write to this PdfRefCountedBuffer
     *
     *  \see PdfRefCountedBuffer
     */
    PdfOutputDevice( PdfRefCountedBuffer* pOutBuffer );

    /** Destruct the PdfOutputDevice object and close any open files.
     */
    virtual ~PdfOutputDevice();

    /** The number of bytes written to this object.
     *  \returns the number of bytes written to this object.
     *  
     *  \see Init
     */
    virtual inline size_t GetLength() const;

    /** Write to the PdfOutputDevice. Usage is as the usage of printf.
     * 
     *  WARNING: Do not use this for doubles or floating point values
     *           as the output might depend on the current locale.
     *
     *  \param pszFormat a format string as you would use it with printf
     *  \returns ErrOk on success
     *
     *  \see Write
     */
    virtual void Print( const char* pszFormat, ... );

    /** Write data to the buffer. Use this call instead of Print if you 
     *  want to write binary data to the PdfOutputDevice.
     *
     *  \param pBuffer a pointer to the data buffer
     *  \param lLen write lLen bytes of pBuffer to the PdfOutputDevice
     *  \returns ErrOk on success
     * 
     *  \see Print
     */
    virtual void Write( const char* pBuffer, size_t lLen );

    /** Seek the device to the position offset from the begining
     *  \param offset from the beginning of the file
     */
    virtual void Seek( size_t offset );

    /** Get the current offset from the beginning of the file.
     *  \return the offset form the beginning of the file.
     */
    virtual inline size_t Tell() const;

    /** Flush the output files buffer to disk if this devices
     *  operates on a disk.
     */
    virtual void Flush();

 private: 
    /** Initialize all private members
     */
    void Init();

 protected:
    size_t        m_ulLength;

 private:
    FILE*                m_hFile;
    char*                m_pBuffer;
    size_t        m_lBufferLen;

    std::ostream*        m_pStream;
    PdfRefCountedBuffer* m_pRefCountedBuffer;
    size_t        m_ulPosition;

};

// -----------------------------------------------------
// 
// -----------------------------------------------------
size_t PdfOutputDevice::GetLength() const
{
    return m_ulLength;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
size_t PdfOutputDevice::Tell() const
{
    return m_ulPosition;
}

};

#endif // _PDF_OUTPUT_DEVICE_H_

