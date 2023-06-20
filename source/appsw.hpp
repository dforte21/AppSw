#ifndef	APPSW_H
#define	APPSW_H

# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <vector>

# include <unistd.h>
# include <fcntl.h>
# include <openssl/aes.h>
# include <openssl/evp.h>

const uint32_t CRC32_POLYNOMIAL = 0xEDB88320;
const uint32_t CRC32_INITIAL_VALUE = 0xFFFFFFFF;

typedef	enum{
	WRONG_OPT,
	EXIT,
	DELETE,
	UNTAR,
	DECRYPT,
	CRC
}	enumOptions;

using namespace std;

class appSw {
	private:
		map<string, enumOptions>	menuOptions;

		appSw();

		void	appDelete( string fileName );
		void	appUntar( string fileName );
		void	appDecrypt( string fileName );
		void	appCRC( string fileName );
		void	appRun( string fileName );

	public:
		appSw( string PATH );
		~appSw();
		
				void	runMenu();
		friend	uint32_t getFileCRC32(const std::string& filename);

		class	pathNotFound : public exception {
			public:
				virtual const char	*what() const throw(){
					return ("Error: Path not found\n");
				}
		};
};

uint32_t calculateCRC32Byte(uint32_t crc, uint8_t data);

uint32_t calculateCRC32(const char* data, size_t length);

uint32_t getFileCRC32(const std::string& filename);

#endif