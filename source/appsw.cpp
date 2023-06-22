#include "appsw.hpp"

const	unsigned char key[]	= "\xD7\x4F\xF0\xEE\x8D\xA3\xB9\x80\x6B\x18\xC8\x77\xDB\xF2\x9B\xBD";
const	unsigned char iv[]	= "\xE5\x0B\x5B\xD8\xE4\xDA\xD7\xA3\xA7\x25\x00\x0F\xEB\x82\xE8\xF1";

appSw::appSw( void ){};

appSw::appSw( string PATH ){

        cout << "Opening " << PATH << ".";
        cout << ".";
        cout << ". ";

        if (chdir(PATH.c_str()) == -1)
                throw (pathNotFound());

        menuOptions.insert(pair<string, enumOptions>("exit", EXIT));
        menuOptions.insert(pair<string, enumOptions>("delete", DELETE));
        menuOptions.insert(pair<string, enumOptions>("untar", UNTAR));
        menuOptions.insert(pair<string, enumOptions>("decrypt", DECRYPT));
        menuOptions.insert(pair<string, enumOptions>("crc", CRC));

        cout << "Successfull\n";
		runMenu();
		cout << endl;
};

appSw::~appSw( void ){
        menuOptions.clear();
};

void    appSw::runMenu(){
        string          input(""), option(""), fileName("");
        enumOptions     opt = WRONG_OPT;

        do{
                cout << "\nEXIT, DELETE, UNTAR, DECRYPT, CRC\nInsert one option and specify a file (lowercase): ";
                getline(cin, input, '\n');
                if (!input.length())
                        return ;

                istringstream iss(input);

                iss >> option >> fileName;

                opt = menuOptions[option];

                if (opt != EXIT && opt != WRONG_OPT && access(fileName.c_str(), F_OK) == -1){
					cerr << "ERROR: File not found\n";
                    continue;
				}

                switch (opt)
                {
                        case EXIT:
                                return ;

                        case DELETE:
                                appDelete(fileName);
                                break ;

                        case UNTAR:
                                appUntar(fileName);
                                break ;

                        case DECRYPT:
                                appDecrypt(fileName);
                                break ;

                        case CRC:
                                appCRC(fileName);
                                break ;

                        default:
                                cout << "Wrong option, please retry: ";
                                break;
                }
        }
        while (1);
}

void    appSw::appDelete( string fileName ){
        remove(fileName.c_str());
        cout << "File deleted\n";
}

void    appSw::appUntar( string fileName ){
        string  cmd("tar -xf ");

        cmd += fileName;
        system(cmd.c_str());
        cout << "File extracted\n";
}

void    appSw::appDecrypt( string fileName ){
	FILE* fin = fopen(fileName.c_str(), "rb");
    if (fin == NULL) {
        printf("Impossibile aprire il file di input.\n");
        return ;
    }

    FILE* fout = fopen("Trustboot.tar", "wb");
    if (fout == NULL) {
        printf("Impossibile creare il file di output.\n");
        fclose(fin);
        return ;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("Errore durante l'inizializzazione del contesto di crittografia.\n");
        fclose(fin);
        fclose(fout);
        return ;
    }

    unsigned char inbuf[4096];
    unsigned char outbuf[4096 + 16];

    int bytesRead, outlen;
    int totalBytesWritten = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);

    while (1) {
        bytesRead = fread(inbuf, sizeof(unsigned char), 4096, fin);
        EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf, bytesRead);
        fwrite(outbuf, sizeof(unsigned char), outlen, fout);
        totalBytesWritten += outlen;

        if (bytesRead < 4096)
            break;
    }

    EVP_DecryptFinal_ex(ctx, outbuf, &outlen);
    fwrite(outbuf, sizeof(unsigned char), outlen, fout);
    totalBytesWritten += outlen;

    EVP_CIPHER_CTX_free(ctx);

    fclose(fin);
    fclose(fout);

    cout << "File decrypted\n";
}

void    appSw::appCRC( string fileName ){
    string crcFilename = "crc.txt";

    ifstream crcFile(crcFilename);
    if (!crcFile) {
        cerr << "ERROR, crc.txt not found\n";
        return ;
    }

    string line;
    if (getline(crcFile, line)) {
        istringstream iss(line);
        uint32_t expectedCRC;
        if (iss >> hex >> expectedCRC) {
            uint32_t    fileCrc = getFileCRC32(fileName);
                        if (fileCrc != expectedCRC)
                                cerr << "ERROR: file has wrong checksum. Calculated: (" << fileCrc << "), Expected (" << expectedCRC << ")\n";
                        else{
                                cout << "Checksum ok, executing trustboot\n";
                                appRun(fileName);}
        } else {
            cerr << "ERROR:Invalid checksum\n";
        }
    }
}

void    appSw::appRun( string fileName ){
        string cmd("./");

        cmd += fileName;
        system("chmod 777 trustboot");
        system(cmd.c_str());
}