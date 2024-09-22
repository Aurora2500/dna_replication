#pragma once

enum class NucleobaseType {
	Adenine = 0,
	Thymine = 1,
	Guanine = 2,
	Cytosine = 3,
};

NucleobaseType letter_to_nucleobase_type(char letter);

class Nucleobase {
private:
	NucleobaseType m_type;
public:
	Nucleobase(NucleobaseType type);
	Nucleobase(char letter);

	Nucleobase complement() const;

	bool operator==(const Nucleobase& rhs) const;

	static const Nucleobase Adenine;
	static const Nucleobase Thymine;
	static const Nucleobase Guanine;
	static const Nucleobase Cytosine;
};