

extern const unsigned int CrcTable[];
class Crc32
{
private:
	static const unsigned int CrcSeed ;

	unsigned int crc ; // crc data checksum so far.


public:
	Crc32();


	unsigned int GetCrc32Value()
	{
		return crc;
	}


	static unsigned int GetCRC32(const char* parr, int nBytes)
	{
		unsigned int unRes = 0;
		if ( nBytes > 0 )
		{
			Crc32 crc32 ;
			crc32.Update(parr,nBytes, 0, nBytes);
			unRes = crc32.GetCrc32Value();
		}
		return unRes;
	}

	/// <summary>
	/// Resets the CRC32 data checksum as if no update was ever called.
	/// </summary>
	 void Reset()
	{
		crc = 0;
	}

	/// <summary>
	/// Updates the checksum with the int bval.
	/// </summary>
	/// <param name = "bval">
	/// the byte is taken as the lower 8 bits of bval
	/// </param>
	 void Update(int bval)
	{
		crc ^= CrcSeed;
		crc = CrcTable[(crc ^ bval) & 0xFF] ^ (crc >> 8);
		crc ^= CrcSeed;
	}

	/// <summary>
	/// Updates the checksum with the bytes taken from the array.
	/// </summary>
	/// <param name="buffer">
	/// buffer an array of bytes
	/// </param>
	 void Update(const char* pbuffer,int nBytes)
	{
		Update(pbuffer,nBytes, 0, nBytes);
	}

	/// <summary>
	/// Adds the byte array to the data checksum.
	/// </summary>
	/// <param name = "buf">
	/// the buffer which contains the data
	/// </param>
	/// <param name = "off">
	/// the offset in the buffer where the data starts
	/// </param>
	/// <param name = "len">
	/// the length of the data
	/// </param>
	 void Update(const char* pbuffer,int nBytes, int off, int len)
	{
		if (pbuffer == 0)
		{
			return;
		}

		if (off < 0 || len < 0 || off + len > nBytes)
		{
			//throw ("len err");
			return;
		}

		crc ^= CrcSeed;

		while (--len >= 0)
		{
			crc = CrcTable[(crc ^ pbuffer[off++]) & 0xFF] ^ (crc >> 8);
		}

		crc ^= CrcSeed;
	}

};
