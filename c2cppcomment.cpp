#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

std::string ltrim( const std::string &s ) {
    return std::regex_replace( s, std::regex( "^\\s+" ), "" );
}
 
std::string rtrim( const std::string &s ) {
    return std::regex_replace( s, std::regex("\\s+$"), "" );
}

void translate_c_to_cpp_comment( std::istream &inp_stream, std::ostream &out_stream );

int main( int argc, char const* argv[] ) {
    if ( argc == 1 ) {
        std::cout << "Translate C-style comments to C++-style comments." << std::endl;
        std::cout << "\nUsage: " << argv[0] << " src [dst]\n" << std::endl;
        std::cout << "    src    : input file contains c-style comments" << std::endl;
        std::cout << "    dst    : optional file for output" << std::endl;
        std::cout << "             if dst is not provided, the standard output will be used instead." << std::endl;
        return EXIT_SUCCESS;
    }

    std::ifstream inp_fs { argv[1] };
    if ( !inp_fs.is_open() ) {
        std::cerr << "Couldn't read file: '" << argv[1] << "'" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream out_fs;
    if ( argc > 2 ) {
        out_fs.open( argv[2] );
        if ( !out_fs.is_open() ) {
            std::cerr << "Couldn't write file: '" << argv[2] << "'" <<  std::endl;
            std::cerr << "Write to the standard output instead!" << std::endl;
        }
    }

    std::ostream &out_stream { out_fs.is_open() ? out_fs : std::cout };

    translate_c_to_cpp_comment( inp_fs, out_stream );

    return EXIT_SUCCESS;
}

void translate_c_to_cpp_comment( std::istream &inp_stream, std::ostream &out_stream ) {
    const std::string inp_str { std::istreambuf_iterator<char> { inp_stream }, {} };
    // regex for capturing c-style comments:
    // (1)(2)(3)    <-- three sub-matches capturing c-style comments
    // (1)          --> single-line text right before "/*" if available
    //    (2)       --> single-line/multi-lines text between the "/*" and "*/"
    //       (3)    --> single-line text right after "*/" if available
    const std::regex re_block( R"((.*)/\*([\s\S]*?)\*/(.*))" );
    const std::sregex_iterator last;
    std::sregex_iterator next( inp_str.begin(), inp_str.end(), re_block );
    std::smatch sm0;
    while ( next != last ) {
        sm0 = *next++; // iterate over comment blocks

        // drop a redundant newline character remaining by preceded match if available 
        if ( sm0.prefix().length() > 0 ) out_stream << sm0.prefix().str().c_str() + 1;

        if ( sm0.str().find( '\n' ) == std::string::npos ) {
            std::string sm3 { ltrim( rtrim( sm0[3].str() ) ) };
            if ( sm3.length() <= 0 ) {
                // here the comment block would be at the end of line
                if ( ltrim( rtrim( sm0[1].str() ) ).length() > 0 )
                    out_stream << rtrim( sm0[1].str() ) << " ";
                out_stream << "//";
                std::string sm2 { ltrim( rtrim( sm0[2].str() ) ) };
                if ( sm2.length() > 0 ) out_stream << " " << sm2;
                out_stream << std::endl;
            } else {
                // the block comment would not at the end of line, don't touch
                out_stream << sm0.str() << std::endl;
            }
        } else {
            // to process comments span multi-lines

            // if sub-match-1 available, remove trailing white spaces and put
            // a single space before the comment contents in sub-match-2 next
            const std::string sm1 { rtrim( sm0[1].str() ) };
            if ( sm1.length() > 0 ) out_stream << sm1 << " ";

            // iterate over lines of multi-lines comment contents and translate
            // into c++-style comments.
            // remark the special case when getline and reach eof as it is an
            // empty line to be commented
            std::string line;
            std::stringstream ss { sm0[2].str() };
            bool reach_eof_at_newline { false };
            while ( std::getline( ss, line ) ) {
                line = ltrim( rtrim( line ) );
                out_stream << "//";
                if ( line.length() > 0 ) out_stream << " " << line;
                out_stream << std::endl;
                reach_eof_at_newline = ss.eof();
            }
            if ( !reach_eof_at_newline ) out_stream << "//" << std::endl;

            // if sub-match-3 available, put it on next new line after remove
            // trailing white spaces
            const std::string sm3 { ltrim( sm0[3].str() ) };
            if ( sm3.length() > 0 ) out_stream << sm3 << std::endl;
        }
    }
    // drop a redundant newline character remaining by preceded match if available 
    if (sm0.suffix().length() > 0 ) out_stream << sm0.suffix().str().c_str() + 1;
}
