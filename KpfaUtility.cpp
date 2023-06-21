/*
 * KpfaUtility.cpp
 *
 *  Created on: 2014. 5. 23.
 *      Author: Youngsun Han
 */

#include "KpfaUtility.h"

/*
 * This function will be used to trim the white spaces
 * at the left side of the given string.
 *
 * @param s input string
 * @return trimmed string
 */
static inline string &
ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

/*
 * This function will be used to trim the white spaces
 * at the right side of the given string.
 *
 * @param s input string
 * @return trimmed string
 */
static inline string &
rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

/*
 * This function will trim the white spaces at both of the sides of the given string.
 *
 * @param s input string
 * @return trimmed string
 */
static inline string &
trim(string &s) {
	return ltrim(rtrim(s));
}

/**
 * This function will separate the given string with the delimiters
 * into a list of the string tokens.
 *
 * @param rString input string
 * @param rToken a list of output string tokens
 * @param rDelimiters delimiters to separate the input string
 * @return the number of output string tokens
 */
uint32_t
KpfaTokenize(const string &rString, KpfaStringList_t &rTokens, const string &rDelimiters) {

    // Skip delimiters at beginning.
    string::size_type lastPos = rString.find_first_not_of(rDelimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = rString.find_first_of(rDelimiters, lastPos);

    // Clear token list
    rTokens.clear();

    while (string::npos != pos || string::npos != lastPos) {

    	string token = rString.substr(lastPos, pos - lastPos);

    	// Ignore the comment data
    	string::size_type pos_comment = token.find('/');

    	if(pos_comment != string::npos) {
    		token = token.substr(0, pos_comment);
    	}

        // Found a token, add it to the vector.
    	rTokens.push_back(trim(token));
        // Skip rDelimiters.  Note the "not_of"
        lastPos = rString.find_first_not_of(rDelimiters, pos);
        // Find next "non-delimiter"
        pos = rString.find_first_of(rDelimiters, lastPos);
    }

    return (uint32_t)rTokens.size();
}

/**
 * This function will remove small quotes from the given string.
 *
 * @param rString input string
 */
string &
KpfaStripQuotes(string &rString) {
    rString.erase(remove(rString.begin(), rString.end(), '\''), rString.end());
    return trim(rString);
}

/**
 * This function will be used to print out the value of the given complex number.
 *
 * @param rOut output stream
 * @param pNumber a complex number
 * @return output stream
 */
ostream &operator << (ostream &rOut, KpfaComplex_t &pNumber) {

	rOut << pNumber.real();
	rOut << ((pNumber.imag() < 0.0) ? "-j" : "+j");
	rOut << fabs(pNumber.imag());

	return rOut;
}
