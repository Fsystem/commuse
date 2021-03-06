/** 
*   @FileName  : LibInterface.cpp
*   @Author    : Double Sword
*   @date      : 2012-5-25
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/

#ifdef ZIP_DLL
#	define EXPORT_DLL _declspec(dllexport)
#else
#	define EXPORT_DLL
#endif

#include <direct.h>
#include <errno.h >
// #if !defined(ZLIB_WINAPI)
// #	define ZLIB_WINAPI
// #endif

#include "zlib_src/unzip.h"
#include "zlib_src/zip.h"

#include <vector>
#include <list>
#include "ILibInterface.h"

#include<crtdbg.h>
#include <assert.h>
#include <stdlib.h>
#include <tchar.h>

//#include "crc32/Crc.h"
//#include <atlconv.h>

#pragma region 解压

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)


static void Display64BitsSize(ZPOS64_T n, int size_char)
{
	/* to avoid compatibility problem , we do here the conversion */
	char number[21];
	int offset=19;
	int pos_string = 19;
	number[20]=0;
	for (;;) {
		number[offset]=(char)((n%10)+'0');
		if (number[offset] != '0')
			pos_string=offset;
		n/=10;
		if (offset==0)
			break;
		offset--;
	}
	{
		int size_display_string = 19-pos_string;
		while (size_char > size_display_string)
		{
			size_char--;
			printf(" ");
		}
	}

	printf("%s",&number[pos_string]);
}

static int do_list(unzFile uf)
{
	uLong i;
	unz_global_info64 gi;
	int err;

	err = unzGetGlobalInfo64(uf,&gi);
	if (err!=UNZ_OK)
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);
	printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
	printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");
	for (i=0;i<gi.number_entry;i++)
	{
		char filename_inzip[256];
		unz_file_info64 file_info;
		uLong ratio=0;
		const char *string_method;
		char charCrypt=' ';
		err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			break;
		}
		if (file_info.uncompressed_size>0)
			ratio = (uLong)((file_info.compressed_size*100)/file_info.uncompressed_size);

		/* display a '*' if the file is crypted */
		if ((file_info.flag & 1) != 0)
			charCrypt='*';

		if (file_info.compression_method==0)
			string_method="Stored";
		else
			if (file_info.compression_method==Z_DEFLATED)
			{
				uInt iLevel=(uInt)((file_info.flag & 0x6)/2);
				if (iLevel==0)
					string_method="Defl:N";
				else if (iLevel==1)
					string_method="Defl:X";
				else if ((iLevel==2) || (iLevel==3))
					string_method="Defl:F"; /* 2:fast , 3 : extra fast*/
			}
			else
				if (file_info.compression_method==Z_BZIP2ED)
				{
					string_method="BZip2 ";
				}
				else
					string_method="Unkn. ";

		Display64BitsSize(file_info.uncompressed_size,7);
		printf("  %6s%c",string_method,charCrypt);
		Display64BitsSize(file_info.compressed_size,7);
		printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
			ratio,
			(uLong)file_info.tmu_date.tm_mon + 1,
			(uLong)file_info.tmu_date.tm_mday,
			(uLong)file_info.tmu_date.tm_year % 100,
			(uLong)file_info.tmu_date.tm_hour,(uLong)file_info.tmu_date.tm_min,
			(uLong)file_info.crc,filename_inzip);
		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	return 0;
}




static int mymkdir(const char* dirname)
{
	int ret=0;
#ifdef _WIN32
	ret = _mkdir(dirname);
#else
#ifdef unix
	ret = mkdir (dirname,0775);
#endif
#endif
	int err = errno;
	return ret;
}



static int makedir (char *newdir)
{
	char *buffer ;
	char *p;
	int  len = (int)strlen(newdir);

	if (len <= 0)
		return 0;

	buffer = (char*)malloc(len+1);
	if (buffer==NULL)
	{
		printf("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}
	strcpy(buffer,newdir);

	if (buffer[len-1] == '/') {
		buffer[len-1] = '\0';
	}
	if (mymkdir(buffer) == 0)
	{
		free(buffer);
		return 1;
	}

	p = buffer+1;
	while (1)
	{
		char hold;

		while(*p && *p != '\\' && *p != '/')
			p++;
		hold = *p;
		*p = 0;
		if ((mymkdir(buffer) == -1) && (errno == ENOENT))
		{
			printf("couldn't create directory %s\n",buffer);
			free(buffer);
			return 0;
		}
		if (hold == 0)
			break;
		*p++ = hold;
	}
	free(buffer);
	return 1;
}





/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
static void change_file_date( const char *filename,
					  uLong dosdate,
					  tm_unz tmu_date
					  )
{
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
                      0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
#ifdef unix
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
#endif
}

static int do_extract_currentfile(
						   unzFile uf,
						   const int* popt_extract_without_path,
						   int* popt_overwrite,
						   const char* password,
						   const char* outpath
						   )
{
	char filename_inzip[256];
	char* filename_withoutpath;
	char* p;
	int err=UNZ_OK;
	FILE *fout=NULL;
	void* buf;
	uInt size_buf;

	unz_file_info file_info;
	uLong ratio=0;
	err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

	char szOutPath[512]={0};
	if(outpath)
	{
		strcpy(szOutPath,outpath);
		int nOutPathLen = strlen(outpath);
		if(nOutPathLen < sizeof(szOutPath))
		{
			if(outpath[nOutPathLen-1]=='\\' || outpath[nOutPathLen-1]=='/')
			{
				strcat(szOutPath,filename_inzip);
			}
			else
			{
#ifdef WIN32
				szOutPath[nOutPathLen]='\\';
#else
				szOutPath[nOutPathLen]='/';
#endif

				strcpy(szOutPath+nOutPathLen+1,filename_inzip);
			}
		}
	}

	if (err!=UNZ_OK)
	{
		printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
		return err;
	}

	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf==NULL)
	{
		printf("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}

	p = filename_withoutpath = filename_inzip;
	while ((*p) != '\0')
	{
		if (((*p)=='/') || ((*p)=='\\'))
			filename_withoutpath = p+1;
		p++;
	}

	if ((*filename_withoutpath)=='\0')
	{
		if ((*popt_extract_without_path)==0)
		{
			printf("creating directory: %s\n",filename_inzip);
			if(outpath)
			{
				mymkdir(szOutPath);
			}
			else mymkdir(filename_inzip);
		}
	}
	else
	{
		const char* write_filename;
		int skip=0;
		if(outpath) write_filename = szOutPath;
		else if ((*popt_extract_without_path)==0)
			write_filename = filename_inzip;
		else
			write_filename = filename_withoutpath;

		err = unzOpenCurrentFilePassword(uf,password);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
		}

		if (((*popt_overwrite)==0) && (err==UNZ_OK))
		{
			char rep=0;
			FILE* ftestexist;
			ftestexist = fopen64(write_filename,"rb");
			if (ftestexist!=NULL)
			{
				fclose(ftestexist);
				do
				{
					char answer[128];
					int ret;

					printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ",write_filename);
					ret = scanf("%1s",answer);
					if (ret != 1)
					{
						exit(EXIT_FAILURE);
					}
					rep = answer[0] ;
					if ((rep>='a') && (rep<='z'))
						rep -= 0x20;
				}
				while ((rep!='Y') && (rep!='N') && (rep!='A'));
			}

			if (rep == 'N')
				skip = 1;

			if (rep == 'A')
				*popt_overwrite=1;
		}

		if ((skip==0) && (err==UNZ_OK))
		{
			fout=fopen64(write_filename,"wb");

			/* some zipfile don't contain directory alone before file */
			if ((fout==NULL) && ((*popt_extract_without_path)==0) &&
				(filename_withoutpath!=(char*)filename_inzip))
			{
				char c=*(filename_withoutpath-1);
				*(filename_withoutpath-1)='\0';
				makedir((char*)write_filename);
				*(filename_withoutpath-1)=c;
				fout=fopen64(write_filename,"wb");
			}

			if (fout==NULL)
			{
				printf("error opening %s\n",write_filename);
			}
		}

		if (fout!=NULL)
		{
			printf(" extracting: %s\n",write_filename);

			do
			{
				err = unzReadCurrentFile(uf,buf,size_buf);
				if (err<0)
				{
					printf("error %d with zipfile in unzReadCurrentFile\n",err);
					break;
				}
				if (err>0)
					if (fwrite(buf,err,1,fout)!=1)
					{
						printf("error in writing extracted file\n");
						err=UNZ_ERRNO;
						break;
					}
			}
			while (err>0);
			if (fout)
				fclose(fout);

			if (err==0)
				change_file_date(write_filename,file_info.dosDate,
				file_info.tmu_date);
		}

		if (err==UNZ_OK)
		{
			err = unzCloseCurrentFile (uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzCloseCurrentFile\n",err);
			}
		}
		else
			unzCloseCurrentFile(uf); /* don't lose the error */
	}

	free(buf);
	return err;
}

static int do_extract_currentfile(
	unzFile uf,
	const int* popt_extract_without_path,
	int* popt_overwrite,
	const char* password
	)
{
	do_extract_currentfile(uf,popt_extract_without_path,popt_overwrite,password,NULL);
}


static int do_extract(
			   unzFile uf,
			   int opt_extract_without_path,
			   int opt_overwrite,
			   const char* password
			   )
{
	uLong i;
	unz_global_info gi;
	int err;
	FILE* fout=NULL;

	unzGoToFirstFile(uf);

	err = unzGetGlobalInfo(uf,&gi);
	if (err!=UNZ_OK)
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);

	for (i=0;i<gi.number_entry;i++)
	{
		if (do_extract_currentfile(uf,&opt_extract_without_path,
			&opt_overwrite,
			password) != UNZ_OK)
			break;

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	return 0;
}

static int do_extract1(unzFile uf,const char* pPwd )
{
	int done, i;
	char szZipName[_MAX_PATH];
	unz_file_info file_info;
	void* ptr = NULL;
	done=unzGoToFirstFile(uf);


		while(done==UNZ_OK)
		{
			unzGetCurrentFileInfo(uf, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
			strupr(szZipName);
			for(i=0; szZipName[i]; i++)	{ if(szZipName[i]=='/') szZipName[i]='\\'; }
			if(unzOpenCurrentFilePassword(uf, pPwd) != UNZ_OK)
			{
				unzClose(uf);
				break;
			}

			ptr = malloc(file_info.uncompressed_size);
			if(!ptr)
			{
				unzCloseCurrentFile(uf);
				unzClose(uf);
				break;
			}

			if(unzReadCurrentFile(uf, ptr, file_info.uncompressed_size) < 0)
			{
				unzCloseCurrentFile(uf);
				unzClose(uf);
				free(ptr);

				return 0;
			}
			unzCloseCurrentFile(uf);

			done=unzGoToNextFile(uf);
		}
		
		
	
	return 0;
}

static int do_extract2(
	unzFile uf,
	int opt_extract_without_path,
	int opt_overwrite,
	const char* password,
	const char* outpath
	)
{
	uLong i;
	unz_global_info gi;
	int err;
	FILE* fout=NULL;

	unzGoToFirstFile(uf);

	err = unzGetGlobalInfo(uf,&gi);
	if (err!=UNZ_OK)
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);

	for (i=0;i<gi.number_entry;i++)
	{
		if (do_extract_currentfile(uf,&opt_extract_without_path,
			&opt_overwrite,
			password,outpath) != UNZ_OK)
			break;

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	return 0;
}


static int do_extract_onefile(
					   unzFile uf,
					   const char* filename,
					   int opt_extract_without_path,
					   int opt_overwrite,
					   const char* password
					   )

{
	int err = UNZ_OK;
	if (unzLocateFile(uf,filename,CASESENSITIVITY)!=UNZ_OK)
	{
		printf("file %s not found in the zipfile\n",filename);
		return 2;
	}

	if (do_extract_currentfile(uf,&opt_extract_without_path,
		&opt_overwrite,
		password) == UNZ_OK)
		return 0;
	else
		return 1;
}

#pragma endregion


#pragma region 压缩

static uLong filetime(char *f,                /* name of file to get info on */
			   tm_zip *tmzip,             /* return value: access, modific. and creation times */
			   uLong *dt             /* dostime */
			   )
{
	int ret = 0;
	{
		FILETIME ftLocal;
		HANDLE hFind;
		WIN32_FIND_DATAA ff32;

		hFind = FindFirstFileA(f,&ff32);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
			FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
			FindClose(hFind);
			ret = 1;
		}
	}
	return ret;
}


/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
static int getFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc)
{
   unsigned long calculate_crc=0;
   int err=ZIP_OK;
   FILE * fin = fopen64(filenameinzip,"rb");
   unsigned long size_read = 0;
   unsigned long total_read = 0;
   if (fin==NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                printf("error in reading %s\n",filenameinzip);
                err = ZIP_ERRNO;
            }

            if (size_read>0)
                calculate_crc = crc32(calculate_crc,(const Bytef*)buf,size_read);
            total_read += size_read;

        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    *result_crc=calculate_crc;
    printf("file %s crc %lx\n", filenameinzip, calculate_crc);
    return err;
}

static int isLargeFile(const char* filename)
{
  int largeFile = 0;
  ZPOS64_T pos = 0;
  FILE* pFile = fopen64(filename, "rb");

  if(pFile != NULL)
  {
    int n = fseeko64(pFile, 0, SEEK_END);

    pos = ftello64(pFile);

                printf("File : %s is %lld bytes\n", filename, pos);

    if(pos >= 0xffffffff)
     largeFile = 1;

                fclose(pFile);
  }

 return largeFile;
}


#pragma endregion



class LibZipFile
{
	typedef struct
	{
		char			_szFileName[_MAX_PATH];
		unz_file_info64 _ZipInfo;
	}MyZipFileinfo;

	typedef struct
	{
		char				_szFileName[_MAX_PATH];
		std::vector<char>	_buf;
	}MyZipFileBuffer;

	typedef std::list<MyZipFileinfo>	ZIPFILELIST;

	typedef std::list<MyZipFileBuffer>	ZIPFILEBUFFERLIST;
public:

	// 打开zip文件
	bool OpenZip(LPCSTR sourcrZipPath,LPCSTR password)
	{
		assert(sourcrZipPath);if(!sourcrZipPath) return 0;
		//USES_CONVERSION;
		
		char filename_try[MAXFILENAME+16] = "";
		uLong i;
		unz_global_info64 gi;
		int err;
		int done;

		strcpy(_filepath,sourcrZipPath);
		if( !password ) memset(_pwd,0,sizeof(_pwd));
		else strcpy(_pwd,password);
		
		_f = unzOpen(sourcrZipPath);
		if (NULL == _f )
		{
			return true;
		}

		err = unzGetGlobalInfo64(_f,&gi);
		assert(UNZ_OK == err);
		if(UNZ_OK != err) 
		{
			unzClose(_f);
			_f = 0;
		}
		else
		{
			//指针指向头
			done=unzGoToFirstFile(_f);
			assert(UNZ_OK == done );
			if(UNZ_OK != done) goto ERROR2;

			MyZipFileinfo	MyZip={0};
			char szFileName[_MAX_PATH]={0};

			for (i=0;i<gi.number_entry;i++)
			{
				err = unzGetCurrentFileInfo64(_f,&MyZip._ZipInfo,szFileName,sizeof(szFileName),NULL,0,NULL,0);
				strcpy_s(MyZip._szFileName,_countof(MyZip._szFileName),(szFileName) );
				if (err!=UNZ_OK) goto ERROR2;

				_FileInfoList.push_back(MyZip);

				if ( (i+1) < gi.number_entry )
				{
					err = unzGoToNextFile(_f);
					if (err!=UNZ_OK) goto ERROR2;
					
				}
			}
		}

		return (int)_f;
ERROR2:
		if(_f)unzClose(_f);
		_f = 0;
		_FileInfoList.clear();
		return false;
	}

	//关闭zip
	void CloseZip()
	{
		if (_f)
		{
			unzClose(_f);
			_f = 0;
			_FileInfoList.clear();
		}
	}

	// 对比文件crc
	bool CompareCrc(LPCSTR filename,unsigned long ulCrc)
	{
		bool bRes = true;
		char szFileName[MAX_PATH]={0};
		strcpy_s(szFileName,_countof(szFileName),filename);

		for (ZIPFILELIST::iterator it = _FileInfoList.begin();
			it!=_FileInfoList.end();it++)
		{
			if ( !stricmp(szFileName,(*it)._szFileName) 
				&& (*it)._ZipInfo.crc == ulCrc )
			{
				bRes = false;
				break;
			}
		}

		return bRes;
	}

	/** 添加需要更新的文件缓冲
	*   @Author : Double sword
	*   @Params : 		
	*   @Return : 
	*   @Date   : 2011-8-16
	*/
	bool AddUpdateFile(LPCSTR filename,char* pBuf,int nLen)
	{
		bool bRes = false;
		assert(filename&& pBuf && nLen > 0); if(!filename&& !pBuf && nLen <= 0) return bRes;

		MyZipFileBuffer myfile;

		strcpy(myfile._szFileName,filename);

 		myfile._buf.insert(myfile._buf.begin(),pBuf,pBuf+nLen );

		_FileBuffer.push_back(myfile);

		return bRes;
	}

	/** 添加完所有的文件调用次函数进行更新zip文件
	*   @Author : Double sword
	*   @Params : 无		
	*   @Return : true成功,false失败
	*   @Date   : 2011-8-16
	*/
	bool UpdateZipFile()
	{
		bool bRes	= true;
		char* pBuf	= NULL;
		zipFile zf	= NULL;
		int err		= ZIP_OK;
		uLong ulCrc = 0;
		int zip64	= 0;
		zip_fileinfo zi;
		memset(&zi,0,sizeof(zi));
		filetime((char*)_filepath,&zi.tmz_date,&zi.dosDate);
		strcat(_filepath,"zip");

		if ( 0 < _FileBuffer.size() )
		{
			//删除残余文件
			FILE* ftestexist=fopen64(_filepath,"rb");
			if (ftestexist)
			{
				fclose(ftestexist);
				//删除文件
				::DeleteFileA(_filepath);
			}

			//打开将要添加到新的zip
			zf = zipOpen64(_filepath,0);
			if (zf == NULL)
			{
				err= ZIP_ERRNO;
			}

			int opt_exclude_path = 1;
			void* buf=NULL;
			int size_buf = 0;
	
#pragma region 第一步从zip中读取不需要更新的文件缓冲
			if (_f && ZIP_OK == err)
			{
				unsigned long crcFile=0;
				uLong i;
				unz_global_info gi;
				FILE* fout=NULL;

				unzGoToFirstFile(_f);
				err = unzGetGlobalInfo(_f,&gi);

				if (err!=UNZ_OK) goto ERROR1;

				char filename_inzip[_MAX_PATH]={0};
				unz_file_info file_info;
				bool bIsUpdate = true;

				for (i=0;i<gi.number_entry;i++)
				{
					bIsUpdate = true;

					int err=UNZ_OK;
					FILE *fout=NULL;
					uLong ratio=0;

					err = unzGetCurrentFileInfo(_f,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
					if (UNZ_OK != err)goto ERROR1;

					for (ZIPFILEBUFFERLIST::iterator it = _FileBuffer.begin();
						it != _FileBuffer.end();it++)
					{
						if ( !_stricmp( (*it)._szFileName ,filename_inzip ) )
						{
							bIsUpdate = false;
							break;
						}
					}
					//原zip中文件如果新添加列表中没有，需要保留
					if (bIsUpdate)
					{
						err = unzOpenCurrentFilePassword(_f,_pwd[0] == 0 ? NULL : _pwd );
						if (UNZ_OK != err)goto ERROR1;

						pBuf = new char[file_info.uncompressed_size];

						int pos = 0;
						char read[WRITEBUFFERSIZE];
						do 
						{
							err = unzReadCurrentFile(_f,read,WRITEBUFFERSIZE);
							if (err<0) goto ERROR1;
							if(err > 0) 
							{
								memcpy(pBuf+pos,read,err);
								pos += err;
							}

						} while (err > 0);

						if (UNZ_OK != err)goto ERROR1;

						err = unzCloseCurrentFile (_f);if (UNZ_OK != err)goto ERROR1;

						err = zipOpenNewFileInZip3_64(zf,filename_inzip,&zi,
							NULL,0,NULL,0,NULL /* comment*/,
							(Z_BEST_COMPRESSION != 0) ? Z_DEFLATED : 0,
							Z_BEST_COMPRESSION,0,
							/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
							-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
							_pwd[0]==0 ? NULL : _pwd ,
							crc32(ulCrc,(const byte*)pBuf,file_info.uncompressed_size), 
							zip64);

						if (UNZ_OK != err)goto ERROR1;

						err = zipWriteInFileInZip (zf,pBuf,file_info.uncompressed_size);
						if (UNZ_OK != err)goto ERROR1;

						err = zipCloseFileInZip(zf);
						if (UNZ_OK != err)goto ERROR1;

						if(pBuf) { delete[] pBuf; pBuf = NULL;}

					}


					if ((i+1)<gi.number_entry)
					{
						err = unzGoToNextFile(_f);
						if (err!=UNZ_OK) goto ERROR1;
					}
				}
			}
#pragma endregion
	
#pragma region 第二步将需要更新的文件从缓冲中写入
			if( ZIP_OK == err )//添加原来zip中没有的资源或者全新的一个zip
			{
				for ( ZIPFILEBUFFERLIST::iterator it = _FileBuffer.begin();
					it != _FileBuffer.end();it++ )
				{
					err = zipOpenNewFileInZip3_64(zf,(*it)._szFileName,&zi,
						NULL,0,NULL,0,NULL /* comment*/,
						(Z_BEST_COMPRESSION != 0) ? Z_DEFLATED : 0,
						Z_BEST_COMPRESSION,0,
						/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
						-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
						_pwd[0]==0 ? NULL : _pwd ,
						crc32(ulCrc,(const byte*)&(*it)._buf[0],(*it)._buf.size()),
						zip64);

					if (UNZ_OK != err)goto ERROR1;

					err = zipWriteInFileInZip ( zf,(const byte*)&(*it)._buf[0],(*it)._buf.size() );
					if (UNZ_OK != err)goto ERROR1;

					err = zipCloseFileInZip( zf );
					if (UNZ_OK != err)goto ERROR1;
				}

				_FileBuffer.clear();


			}
#pragma endregion

			if(zf)zipClose(zf,NULL);
			
			
		}
		return bRes;

ERROR1:
		bRes = false;
		if(pBuf) { delete[] pBuf; pBuf = NULL;}
		return bRes;
	}

	/** 获取文件列表
	*   @FuncName : GetZipFileList
	*   @Author   : Double sword
	*   @Params   : pszList \0分割项 \0\0表示结束	
			
	*   @Return   :	void
							
	*   @Date     :	2012-10-31
	*/
	void GetZipFileList(char* pszList)
	{
		pszList[0]=0;
		pszList[1]=0;
		if(_f)
		{
			char* pTmp = pszList;
			for (ZIPFILELIST::iterator it = _FileInfoList.begin();
				it != _FileInfoList.end();it++)
			{
				strcpy(pTmp,(*it)._szFileName);
				pTmp+=strlen( (*it)._szFileName  )+1;//add '\0' one byte
			}
			*pTmp = 0;
		}
	}

	/** 获取zip中文件的大小
	*   @FuncName : GetZipFileSize
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	
							
	*   @Date     :	2012-10-31
	*/
	int GetZipFileSize(char* pszFileName)
	{
		int nSize = 0;
		if (_f)
		{
			for (ZIPFILELIST::iterator it = _FileInfoList.begin();
				it != _FileInfoList.end();it++)
			{
				if( !stricmp(pszFileName,(*it)._szFileName) )
				{
					nSize = (*it)._ZipInfo.uncompressed_size;
					break;
				}
			}
		}

		return nSize;
	}


	/** ReadFile
	*   @FuncName : ReadZipFile
	*   @Author   : Double sword
	*   @Params   : pszFileName,	pBuf ,lenth of pBuf
			
	*   @Return   :	int copyed bytes
							
	*   @Date     :	2012-10-31
	*/
	int ReadZipFile(char* pszFileName,char* pPwd,void* pBuf,int nBufLen)
	{
		int nSize = 0;
		if (_f)
		{
			int err = ZIP_OK;
			err=unzLocateFile(_f,pszFileName,0);
			if (ZIP_OK == err)
			{
				do 
				{
					err=unzOpenCurrentFilePassword(_f,NULL == pPwd?NULL:pPwd);
					if (ZIP_OK != err) break;
					nSize=unzReadCurrentFile(_f,pBuf,nBufLen);
					err=unzCloseCurrentFile(_f);

				} while (0);
				
			}
		}
		return nSize;
	}

	//更换文件
	void ChangeZipFile()
	{
		//USES_CONVERSION;
		char path[_MAX_PATH]={0};
		memcpy(path,_filepath,strlen(_filepath)-3);
		::DeleteFileA(path);
		::rename(_filepath,path);
	}

	//解压文件
	int UnZip(LPCSTR szUnDesPath,LPCSTR szPwd)
	{
		//if(nZipHandle == 0) return 0;
		return do_extract2(_f,0,1,szPwd,szUnDesPath);
	}

protected:
private:
	char						_filepath[_MAX_PATH];
	char						_pwd[_MAX_PATH];
	unzFile						_f;
	ZIPFILELIST					_FileInfoList;
	ZIPFILEBUFFERLIST			_FileBuffer;
};

	/** 打开zip文件
	*   @Author : Double sword
	*   @Params : sourcrZipPath-zip文件全路径		
	*   @Return : 返回该zip文件的id,关闭时需要传这个参数
	*   @Date   : 2011-8-16
	*/
	HZIPHANDLE EXPORT_DLL ZOpenZip(LPCSTR sourcrZipPath,LPCSTR password)
	{
		LibZipFile* p = new LibZipFile;
		if (p)
		{
			if( !p->OpenZip(sourcrZipPath,password) )
			{
				delete p;
				p=0;
			}
		}
		
		return p;
	}

	/** 关闭zip文件
	*   @Author : Double sword
	*   @Params : fileId-从openzip获得,bHasUpdateOpt是否调用UpdateZipFile
	*   @Return : 无
	*   @Date   : 2011-8-16
	*/
	void EXPORT_DLL ZCloseZip(HZIPHANDLE nZip,bool bHasUpdateOpt/* = true*/)
	{
		assert(nZip); if (!nZip) return;

		LibZipFile* p = (LibZipFile* )nZip;
		p->CloseZip();
		if (bHasUpdateOpt)
		{
			p->ChangeZipFile();
		}
		delete p;
	}

	/** 对比文件crc
	*   @Author : Double sword
	*   @Params : sourcrZipPath-zip文件的路径filename-需要对比的文件名，ulCrc-文件的crc		
	*   @Return : true表示需要更新，false不需要
	*   @Date   : 2011-8-16
	*/
	bool EXPORT_DLL ZCompareCrc(HZIPHANDLE nZip,LPCSTR filename,unsigned long ulCrc)
	{
		assert(nZip&&filename); if (!nZip || !filename) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->CompareCrc(filename,ulCrc);
	}

	/** 添加需要更新的文件缓冲
	*   @Author : Double sword
	*   @Params : 		
	*   @Return : 
	*   @Date   : 2011-8-16
	*/
	bool EXPORT_DLL ZAddUpdateFile(HZIPHANDLE nZip,LPCSTR filename,char* pBuf,int nLen)
	{
		assert(nZip && filename && pBuf && nLen > 0);
		if (!nZip ||!filename || !pBuf || nLen <=0 ) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->AddUpdateFile(filename,pBuf,nLen);
	}

	/** 添加完所有的文件调用次函数进行更新zip文件
	*   @Author : Double sword
	*   @Params : 无		
	*   @Return : true成功,false失败
	*   @Date   : 2011-8-16
	*/
	bool EXPORT_DLL ZUpdateZipFile(HZIPHANDLE nZip)
	{
		assert(nZip); if (!nZip) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->UpdateZipFile();
	}


	/** 获取文件列表
	*   @FuncName : GetZipFileList
	*   @Author   : Double sword
	*   @Params   : pszList \0分割项 \0\0表示结束	
	*   @Return   :	void			
	*   @Date     :	2012-10-31
	*/
	void ZGetZipFileList(HZIPHANDLE nZip,char* pszList)
	{
		assert(nZip); if (!nZip) return ;

		LibZipFile* p = (LibZipFile* )nZip;
		p->GetZipFileList(pszList);
	}


	/** 读取zip文件中的大小
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : 	
	*   @Return   :					
	*   @Date     :	2012-10-31
	*/
	int EXPORT_DLL ZGetZipFileSize(HZIPHANDLE nZip,char* pszFileName)
	{
		assert(nZip); if (!nZip) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->GetZipFileSize(pszFileName);
	}


	/** 读取zip文件字节流
	*   @FuncName : ReadZipFile
	*   @Author   : Double sword
	*   @Params   : nZip,pszFileName,	pBuf ,lenth of pBuf
			
	*   @Return   :	文件的大小
							
	*   @Date     :	2012-10-31
	*/
	int EXPORT_DLL ZReadZipFile(HZIPHANDLE nZip,char* pszFileName,char* pPwd,void* pBuf,int nBufLen)
	{
		assert(nZip); if (!nZip) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->ReadZipFile(pszFileName,pPwd,pBuf,nBufLen);
	}

	/** 解压锁
	*   @Author   : Double sword
	*   @Params   : nZip-zip文件句柄,szZipOutPath-输出路径,szPwd-密码(NULL)
	*   @Return   :				
	*   @Date     :	2016-4-21
	*/
	int EXPORT_DLL ZUnZipFile(HZIPHANDLE nZip,LPCSTR szZipOutPath,LPCSTR szPwd)
	{
		assert(nZip); if (!nZip) return false;

		LibZipFile* p = (LibZipFile* )nZip;
		return p->UnZip(szZipOutPath,szPwd);
	}


// #define ONE 1
// 
// #if ONE == 1
// 	int main()
// 	{
// 		#define __WINDOWS__
// 		#if defined(__WINDOWS__)
// 			_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
// 		#endif
// 
// 		//char szZipFile[MAX_PATH]={0};
// 		//char szZipPwd[MAX_PATH]={0};
// 		//scanf("%s",szZipFile);
// 		//scanf("%s",szZipPwd);
// 		int nZip = OpenZip("1.zip","123456");
// 
// 		int nSize = GetZipFileSize(nZip,"1.jpg");
// 		char * pData = new char[nSize];
// 		int nSize1=ReadZipFile(nZip,"1.jpg","123456",pData,nSize);
// 		delete[] pData;
// 
// 		assert(nSize == nSize1);
// 
// 		CloseZip(nZip,false);
// 		 
// 		printf("%d\n",nSize);
// 
// 		return 0;
// 	}
// #endif
//
//#define ONE 
//
//#ifdef ONE
//
//int main()
//{
//
//#define __WINDOWS__
//#if defined(__WINDOWS__)
//	_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	int nZip = OpenZip( TEXT("D:\\Working\\source\\2008test\\ZLib_Test\\texture.paq"),TEXT("kh?儅?t嬼j婨詪婱詪B�@") );
//	if (nZip)
//	{
//		char* pBuf = NULL;
//		int nSize = 0;
//		FILE* f = fopen("bk99.png","rb");
//		if (f)
//		{
//			fseek(f,0,SEEK_END);
//			nSize = ftell(f);
//			fseek(f,0,SEEK_SET);
//			pBuf = new char[nSize];
//			fread(pBuf,1,nSize,f);
//
//			unsigned long ulcrc = 0;
//			//Crc32 c;
//
//			printf("crc:%X\n",GetCrcLong((const char*)pBuf,nSize));
//			printf("crc:%X\n",crc32(ulcrc,(const byte*)pBuf,nSize) );
//
//		}
//		if( CompareCrc(nZip,"texture/bk99.png",0) )
//		{
//			AddUpdateFile( nZip,"texture/bk99.png",pBuf,nSize );
//		}
//
//		UpdateZipFile(nZip);
//
//		CloseZip(nZip);
//
//		if(pBuf)
//			delete[] pBuf;
//	}
//
//	return 0;
//}
//
//#else
//
//int main()
//{
//
//#define __WINDOWS__
//#if defined(__WINDOWS__)
//	_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//		const char *zipfilename="texture.paq";
//		const char* pwd = "kh?儅?t嬼j婨詪婱詪B�@";
//	
//	#pragma region 解压
//	
//		char filename_try[MAXFILENAME+16] = "";
//		unzFile uf=NULL;
//	
//		uf = unzOpen(zipfilename);
//		if (uf==NULL)
//		{
//			strcpy(filename_try,zipfilename);
//			strcat(filename_try,".zip");
//			uf = unzOpen(filename_try);
//		}
//	
//		if (uf==NULL)
//		{
//			printf("Cannot open %s or %s.zip\n",zipfilename,zipfilename);
//			return 1;
//		}
//		printf("%s opened\n",filename_try);
//	
//		//打印列表
//		do_list(uf);
//		//do_extract1(uf,"kh?儅?t嬼j婨詪婱詪B�@");
//		do_extract( uf,0,1,pwd );//解压
//	
//		unzClose(uf);
//	
//	#pragma endregion
//	//
//	//#pragma region 压缩
//	//
//	//	zipFile zf;
//	//	int errclose;
//	//	const char* filenameinzip = "test.zip";
//	//	const char* password=NULL;
//	//	char filename_try1[MAXFILENAME+16];
//	//	int opt_exclude_path = 1;
//	//	int err = ZIP_OK;
//	//	void* buf=NULL;
//	//	int size_buf = 0;
//	//
//	//	strcpy(filename_try1,filenameinzip);
//	//	FILE* ftestexist;
//	//
//	//	ftestexist = fopen64(filename_try1,"rb");
//	//
//	//	zf = zipOpen64(filename_try1,ftestexist ? 2 : 0);
//	//
//	//	if (zf == NULL)
//	//	{
//	//		printf("error opening %s\n",filename_try1);
//	//		err= ZIP_ERRNO;
//	//	}
//	//	else
//	//		printf("creating %s\n",filename_try1);
//	//
//	//	if (ZIP_OK == err)
//	//	{
//	//		
//	//	FILE * fin;
//	//	int size_read;
//	//
//	//	const char *savefilenameinzip;
//	//	zip_fileinfo zi;
//	//	unsigned long crcFile=0;
//	//	int zip64 = 0;
//	//
//	//	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
//	//		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
//	//	zi.dosDate = 0;
//	//	zi.internal_fa = 0;
//	//	zi.external_fa = 0;
//	//	filetime((char*)filenameinzip,&zi.tmz_date,&zi.dosDate);
//	//
//	//	/*
//	//	err = zipOpenNewFileInZip(zf,filenameinzip,&zi,
//	//	NULL,0,NULL,0,NULL / * comment * /,
//	//	(opt_compress_level != 0) ? Z_DEFLATED : 0,
//	//	opt_compress_level);
//	//	*/
//	//	if ((password != NULL) && (err==ZIP_OK))
//	//		err = getFileCrc(filenameinzip,buf,size_buf,&crcFile);
//	//
//	//	zip64 = isLargeFile(filenameinzip);
//	//
//	//	/* The path name saved, should not include a leading slash. */
//	//	/*if it did, windows/xp and dynazip couldn't read the zip file. */
//	//	savefilenameinzip = filenameinzip;
//	//	while( savefilenameinzip[0] == '\\' || savefilenameinzip[0] == '/' )
//	//	{
//	//		savefilenameinzip++;
//	//	}
//	//
//	//	/*should the zip file contain any path at all?*/
//	//	if( opt_exclude_path )
//	//	{
//	//		const char *tmpptr;
//	//		const char *lastslash = 0;
//	//		for( tmpptr = savefilenameinzip; *tmpptr; tmpptr++)
//	//		{
//	//			if( *tmpptr == '\\' || *tmpptr == '/')
//	//			{
//	//				lastslash = tmpptr;
//	//			}
//	//		}
//	//		if( lastslash != NULL )
//	//		{
//	//			savefilenameinzip = lastslash+1; // base filename follows last slash.
//	//		}
//	//	}
//	//
//	//	/**/
//	//	err = zipOpenNewFileInZip3_64(zf,"1/1.txt"/*savefilenameinzip*/,&zi,
//	//		NULL,0,NULL,0,NULL /* comment*/,
//	//		(Z_BEST_COMPRESSION != 0) ? Z_DEFLATED : 0,
//	//		Z_BEST_COMPRESSION,0,
//	//		/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
//	//		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
//	//		password,crcFile, zip64);
//	//
//	//	if (err != ZIP_OK)
//	//		printf("error in opening %s in zipfile\n",filenameinzip);
//	//	else
//	//	{
//	//		fin = fopen64("1.txt","rb");
//	//		if (fin==NULL)
//	//		{
//	//			err=ZIP_ERRNO;
//	//			printf("error in opening %s for reading\n",filenameinzip);
//	//		}
//	//	}
//	//
//	//	if (err == ZIP_OK)
//	//		do
//	//		{
//	//			err = ZIP_OK;
//	//			fseek(fin,0,SEEK_END);
//	//			size_buf = ftell(fin);
//	//			buf = new char[size_buf];
//	//			fseek(fin,0,SEEK_SET);
//	//			size_read = (int)fread(buf,1,size_buf,fin);
//	//			if (size_read < size_buf)
//	//				if (feof(fin)==0)
//	//				{
//	//					printf("error in reading %s\n",filenameinzip);
//	//					err = ZIP_ERRNO;
//	//				}
//	//
//	//				if (size_read>0)
//	//				{
//	//					err = zipWriteInFileInZip (zf,buf,size_read);
//	//					if (err<0)
//	//					{
//	//						printf("error in writing %s in the zipfile\n",
//	//							filenameinzip);
//	//					}
//	//					size_read = 0;
//	//
//	//				}
//	//		} while ((err == ZIP_OK) && (size_read>0));
//	//
//	//		if (fin)
//	//			fclose(fin);
//	//
//	//		if (err<0)
//	//			err=ZIP_ERRNO;
//	//		else
//	//		{
//	//			err = zipCloseFileInZip(zf);
//	//			if (err!=ZIP_OK)
//	//				printf("error in closing %s in the zipfile\n",
//	//				filenameinzip);
//	//		}
//	//	}
//	//
//	//
//	//
//	//	if (zf)
//	//	{
//	//		errclose = zipClose(zf,NULL);
//	//		if (errclose != ZIP_OK) printf("error in closing %s\n",filename_try1);
//	//	}
//	//	
//	//	if (buf)
//	//	{
//	//		free(buf);
//	//		buf = NULL;
//	//	}
//	//	
//	//	
//	//#pragma endregion
//
//	return 0;
//}

//#endif