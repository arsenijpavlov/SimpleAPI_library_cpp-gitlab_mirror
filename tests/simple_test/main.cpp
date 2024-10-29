#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


std::string preview_key = "b3BlbnNzaC1rZXktdjEAAAAABG5vbmUAAAAEbm9uZQAAAAAAAAABAAABlwAAAAdzc2gtcn"
                          "NhAAAAAwEAAQAAAYEA2pU/AHXypBpATryHXEe0fcSImJHX0LePzG2axDMrDbZNxe9DeKFT"
                          "SBqPQ791SghYK76vxAaNhV5oYL31RcSdBGV7Kdo1RuGW23Y2PW1QuSfmB1Dp1zZ98CKFPf"
                          "xguYljUTpdiL7L9zTdBv7xElTc8Anq+jkikUts9S0hT43bj4C/wM4K+F5noIO6zTPwgkzd"
                          "PfumKyw3jmLwTxvRE6+EJVRKAyhxqjvy1DjV8zbyKMHYgoQZ5rRnOjJYVWfsCViAb9HUtU"
                          "nAI09PgLlUBAXkbD9QUp7kQ0+URTePtMTDWdErKDsZcufIYxP/cjXeVsZnmL4mI+sV/lL1"
                          "TECT9XCi1ZlSdWARQKADdo9uirwU0pKkPhoLs7PS2YbIU0qUKwQR0YjeTZtsoRpPa6VB7E"
                          "J93PTx50nj2AtDxdiPiKpNXkR4yJiQuZhu+oacS41vBwTTefX95dz68Hvv1tcTYraFBDAB"
                          "YFm7QSBsrSEElc5c0Oob9WQ4ToxFnYRiyzHyJOpFAAAFmGaJjtRmiY7UAAAAB3NzaC1yc2"
                          "EAAAGBANqVPwB18qQaQE68h1xHtH3EiJiR19C3j8xtmsQzKw22TcXvQ3ihU0gaj0O/dUoI"
                          "WCu+r8QGjYVeaGC99UXEnQRleynaNUbhltt2Nj1tULkn5gdQ6dc2ffAihT38YLmJY1E6XY"
                          "i+y/c03Qb+8RJU3PAJ6vo5IpFLbPUtIU+N24+Av8DOCvheZ6CDus0z8IJM3T37pissN45i"
                          "8E8b0ROvhCVUSgMocao78tQ41fM28ijB2IKEGea0ZzoyWFVn7AlYgG/R1LVJwCNPT4C5VA"
                          "QF5Gw/UFKe5ENPlEU3j7TEw1nRKyg7GXLnyGMT/3I13lbGZ5i+JiPrFf5S9UxAk/VwotWZ"
                          "UnVgEUCgA3aPboq8FNKSpD4aC7Oz0tmGyFNKlCsEEdGI3k2bbKEaT2ulQexCfdz08edJ49"
                          "gLQ8XYj4iqTV5EeMiYkLmYbvqGnEuNbwcE03n1/eXc+vB779bXE2K2hQQwAWBZu0EgbK0h"
                          "BJXOXNDqG/VkOE6MRZ2EYssx8iTqRQAAAAMBAAEAAAGBAJwzRRdeMivk+omJni1qrGent3"
                          "lX12K2mlMbDBsi+aRN35wP1CZzGqGPIUvnmLhz6BYyqUYq7J0fDvX7KThXPWGz52TMI4Bo"
                          "mzZvbmlNIf9+rJoTz1dVAIBHGvJl1PUhYdDdJZQfzH/2zfA+5SUVceEzJTD0/WfY/57hRk"
                          "mkdP9CoUT1uM8uH12MZURY1ok2xm+R0nhNm9HrkBDoLuujZIqMhmtlTny5ZFkV2IZ6gGhx"
                          "xffW0O+VgaML8SXvIrS3AZcRdSf1f+xZGPRKGTE7PSLtnFOYrLuyRNxCp0MhqXtHRyQ4y9"
                          "qW2CrN6D6oXATZp9eX1AZSrb3KwxaHWgViF75Org0BBmha/3dA6zurZ8S1TjeAun422Aui"
                          "twkO0YBJndhI7IK95EQzas212ZH9WC6FXMtxpMPMLa5fJOTdPzJzKA/c3Sak9oXbkUJIRv"
                          "EBSqY3Ast6v+EQsN87hYxB9kvtffgy41IbL51x5N4EIeh2lxxZhVsjhJnbZxzL60T4IQAA"
                          "AMEA0WopuQTo3XNJn/PbBxN/X6k+v9lMIAzyHqIVvFg0K1F0U/qJK+NOK9XfEKHtL3q7ec"
                          "dRgLAYAY518Trb0c8uOi1DbNAse1ifdgG3uacJ2oZWlSoximGJr5u0RzvOnGDb7rVThumd"
                          "8t3Zb6vYv6OFXDjzWffJiw2C4FMyssijvWCzbTV70CKaP4x6ooTX94rU+wp2bM5X/jBikt"
                          "i2nJsNW7A1AtfAm5GhBMI/fE3X2vgD2K4CfdYfO+GW8d7QnFulAAAAwQD1vF1rIyzhequk"
                          "J6P3KjeQSdymxsloleDLy3jNiXIRXArCtVWVJ7Bk3gqgknzkY7qfQW1mvKtIvuYo/IRGxh"
                          "k/+vI+Gyui5xtqEM0FddJy+m2dNlKf7Hx7/t0JYmydbxbYYPYvkGwHkWo44elO0yE2fpJd"
                          "o/Ivbtkbl5hO7ERxT8lmhzDhFFlnrYjsfKNp636ajW6/MWsv0N1WH4v79ROG1CqW5YuetQ"
                          "gjE4enDRJ4gpO1XeADJPXvFRqvimjdQM0AAADBAOO2icYv0yGCOVxkIoywZEAXMYjBpHFf"
                          "7RXigzOOnpkFKNE6vwKVXvlUOmEKnh7vLQfVFz/SBVGuI6EKm7C4iSfoub5Zp8p7qIY8h9"
                          "ZHzy1s1A0v0ufP2jKB7g5MZCVJHol/mkIYlfxh4JhT2ZM/FU5EamOjnT3AgIm0DOCTjnNt"
                          "ufzKOdHj6AkSM95xN+ztMCoHocWWQJB4KP8rqQJMx60lTCo9XiyJlvee2YwnuPe4cLOv0O"
                          "oM+OcllgthnKLvWQAAABpyb290QHJpZGRpY2stTlBPQ29saWJyaS1QQwECAwQFBgc=";
std::string public_key = "AAAAB3NzaC1yc2EAAAADAQABAAABgQDalT8AdfKkGkBOvIdcR7R9xIiYkdfQt4/MbZrEMy"
                         "sNtk3F70N4oVNIGo9Dv3VKCFgrvq/EBo2FXmhgvfVFxJ0EZXsp2jVG4ZbbdjY9bVC5J+YH"
                         "UOnXNn3wIoU9/GC5iWNROl2Ivsv3NN0G/vESVNzwCer6OSKRS2z1LSFPjduPgL/Azgr4Xm"
                         "egg7rNM/CCTN09+6YrLDeOYvBPG9ETr4QlVEoDKHGqO/LUONXzNvIowdiChBnmtGc6MlhV"
                         "Z+wJWIBv0dS1ScAjT0+AuVQEBeRsP1BSnuRDT5RFN4+0xMNZ0SsoOxly58hjE/9yNd5Wxm"
                         "eYviYj6xX+UvVMQJP1cKLVmVJ1YBFAoAN2j26KvBTSkqQ+Gguzs9LZhshTSpQrBBHRiN5N"
                         "m2yhGk9rpUHsQn3c9PHnSePYC0PF2I+Iqk1eRHjImJC5mG76hpxLjW8HBNN59f3l3Prwe+"
                         "/W1xNitoUEMAFgWbtBIGytIQSVzlzQ6hv1ZDhOjEWdhGLLMfIk6kU=";

std::vector<std::string> chipher(std::string str) {
    std::vector<std::string> vec;
    size_t block_len = public_key.length();

    //разбить на куски, соответствующие размеру блока public_key
    while(str.length() > block_len) {
        vec.push_back(str.substr(0, block_len));
        str.erase(0, block_len);
    }
    //дополнить нулями
    if(vec.size() == 0 && str.length() > 0)
        vec.push_back(str);
    while(vec.back().length() < public_key.length())
        vec.back().push_back(0);

    return vec;
}
std::vector<std::string> dechipher(const std::string& str) {
    return {};
}

int main() {
    std::string message1 = "abcdefghijklmnopqrstuvwxyz0123456789!";
    std::vector<std::string> ch_messages = chipher(message1);
    for(std::string fragment : ch_messages)
        std::cout << "chiphered fragment size [" << fragment.length() << "]" << std::endl;
//    std::vector<std::string> message2 = dechipher(compare(ch_messages));
//    for(std::string fragment : ch_messages)
//        std::cout << fragment.length() << std::endl;

//    std::cout << "correct: " << (message1 == message2 ? "true" : "false") << std::endl;

    return 0;
}
