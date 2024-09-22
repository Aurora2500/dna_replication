#include "nucleobase.hpp"

NucleobaseType letter_to_nucleobase_type(char letter) {
	switch (letter) {
		case 'A':
			return NucleobaseType::Adenine;
		case 'T':
			return NucleobaseType::Thymine;
		case 'C':
			return NucleobaseType::Cytosine;
		case 'G':
			return NucleobaseType::Guanine;
	}
}

Nucleobase::Nucleobase(NucleobaseType type)
	: m_type(type)
{ }

Nucleobase::Nucleobase(char letter)
	: m_type(letter_to_nucleobase_type(letter))
{ }

Nucleobase Nucleobase::complement() const {
	return Nucleobase(static_cast<NucleobaseType>(static_cast<int>(m_type) ^ 1));
}

bool Nucleobase::operator==(const Nucleobase& rhs) const {
	return m_type == rhs.m_type;
}

const Nucleobase Nucleobase::Adenine(NucleobaseType::Adenine);
const Nucleobase Nucleobase::Thymine(NucleobaseType::Thymine);
const Nucleobase Nucleobase::Guanine(NucleobaseType::Guanine);
const Nucleobase Nucleobase::Cytosine(NucleobaseType::Cytosine);