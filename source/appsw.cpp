#include "appsw.hpp"

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

        usleep(100000);
        cout << "Successfull\n";
        try {
                runMenu();
        }
        catch (fileNotFound &e){
                throw (fileNotFound());
        }
};

appSw::~appSw( void ){
        menuOptions.clear();
};

void    appSw::runMenu(){
        string          input(""), option(""), fileName("");
        enumOptions     opt = WRONG_OPT;

        do{
                cout << "\nDELETE, UNTAR, DECRYPT, CRC\nInsert one option and specify a file (lowercase): ";
                getline(cin, input, '\n');
                while (input.length() <= 1){
                        if (!input.length())
                                return ;
                        cout << "Insert one option and specify a file (lowercase): ";
                        getline(cin, input, '\n');
                }

                istringstream iss(input);

                iss >> option >> fileName;

                opt = menuOptions[option];

                if (opt != EXIT && opt != WRONG_OPT && access(fileName.c_str(), F_OK) == -1)
                        throw(fileNotFound());

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
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        cout << "0 Decryption error\n";
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL) != 1) {
        cout << "1 Decryption error\n";
    }

    ifstream fin(fileName, ios::binary);
    if (!fin) {
        cout << "2 Decryption error\n";
    }

    ofstream fout("trustboot", ios::binary);
    if (!fout) {
        fin.close();
        cout << "3 Decryption error\n";
    }

    unsigned char inbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    unsigned char outbuf[1024];
    int bytesRead, outLen;

    while (fin.read(reinterpret_cast<char*>(inbuf), sizeof(inbuf))) {
        if (EVP_DecryptUpdate(ctx, outbuf, &outLen, inbuf, fin.gcount()) != 1) {
            fin.close();
            fout.close();
            EVP_CIPHER_CTX_free(ctx);
            cout << "4 Decryption error\n";
        }
        fout.write(reinterpret_cast<char*>(outbuf), outLen);
    }

    EVP_DecryptFinal_ex(ctx, outbuf, &outLen);
    fout.write(reinterpret_cast<char*>(outbuf), outLen);

    fin.close();
    fout.close();
    EVP_CIPHER_CTX_free(ctx);
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
        system(cmd.c_str());
    	cout << "File executed\n";
}