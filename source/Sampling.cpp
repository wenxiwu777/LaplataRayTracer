#include "Common.h"
#include "Sampling.h"

namespace LaplataRayTracer
{
	//
	// SamplerBase
	//
	//
	SamplerBase::SamplerBase(void)
		: mnSampleCount(1), mnSets(83), mlCount(0), mnJump(0)
	{
			mvecSamples_UnitSquare.reserve(mnSampleCount * mnSets);
			SetupShuffleIndices();
	}
	SamplerBase::SamplerBase(const int num)
		: mnSampleCount(num), mnSets(83), mlCount(0), mnJump(0)
	{
		mvecSamples_UnitSquare.reserve(mnSampleCount * mnSets);
		SetupShuffleIndices();
	}
	SamplerBase::SamplerBase(const int num, const int numSet)
		: mnSampleCount(num), mnSets(numSet), mlCount(0), mnJump(0)
	{
		mvecSamples_UnitSquare.reserve(mnSampleCount * mnSets);
		SetupShuffleIndices();
	}
	SamplerBase::SamplerBase(const SamplerBase& s)
		: mnSampleCount(s.mnSampleCount),
		mnSets(s.mnSets),
		mvecSamples_UnitSquare(s.mvecSamples_UnitSquare),
		mvecShuffledIndices(s.mvecShuffledIndices),
		mvecSamples_UnitDisk(s.mvecSamples_UnitDisk),
		mvecSamples_HemiShpere(s.mvecSamples_HemiShpere),
		mvecSamples_Shpere(s.mvecSamples_Shpere),
		mlCount(s.mlCount),
		mnJump(s.mnJump)
	{

	}
	SamplerBase& SamplerBase::operator= (const SamplerBase& rhs)
	{
		if (this == &rhs)
			return (*this);

		mnSampleCount = rhs.mnSampleCount;
		mnSets = rhs.mnSets;
		mvecSamples_UnitSquare = rhs.mvecSamples_UnitSquare;
		mvecShuffledIndices = rhs.mvecShuffledIndices;
		mvecSamples_UnitDisk = rhs.mvecSamples_UnitDisk;
		mvecSamples_HemiShpere = rhs.mvecSamples_HemiShpere;
		mvecSamples_Shpere = rhs.mvecSamples_Shpere;
		mlCount = rhs.mlCount;
		mnJump = rhs.mnJump;

		return (*this);
	}

	SamplerBase::~SamplerBase(void)
	{

	}

	//
	void SamplerBase::SetSetCount(const int np)
	{
		mnSets = np;
	}
	int SamplerBase::GetSampleCount(void) const
	{
		return mnSampleCount;
	}

	void SamplerBase::ShuffleXCoordinates(void)
	{
		for (int p = 0; p < mnSets; p++)
			for (int i = 0; i < mnSampleCount - 1; i++) {
				int target = Random::RandInt() % mnSampleCount + p * mnSampleCount;
				float temp = mvecSamples_UnitSquare[i + p * mnSampleCount + 1].X();
				mvecSamples_UnitSquare[i + p * mnSampleCount + 1].x = mvecSamples_UnitSquare[target].X();
				mvecSamples_UnitSquare[target].x = temp;
			}
	}
	void SamplerBase::ShuffleYCoordinates(void)
	{
		for (int p = 0; p < mnSets; p++)
			for (int i = 0; i < mnSampleCount - 1; i++) {
				int target = Random::RandInt() % mnSampleCount + p * mnSampleCount;
				float temp = mvecSamples_UnitSquare[i + p * mnSampleCount + 1].Y();
				mvecSamples_UnitSquare[i + p * mnSampleCount + 1].y = mvecSamples_UnitSquare[target].Y();
				mvecSamples_UnitSquare[target].y = temp;
			}
	}
	void SamplerBase::SetupShuffleIndices(void)
	{
		mvecShuffledIndices.reserve(mnSampleCount * mnSets);
		vector<int> indices;

		for (int j = 0; j < mnSampleCount; j++)
			indices.push_back(j);

		for (int p = 0; p < mnSets; p++) {
			random_shuffle(indices.begin(), indices.end());

			for (int j = 0; j < mnSampleCount; j++)
				mvecShuffledIndices.push_back(indices[j]);
		}
	}

	void SamplerBase::MapSamplesToUnitDisk(void)
	{
		int size = mvecSamples_UnitSquare.size();
		float r, phi;		// polar coordinates
		Point2f sp; 		// sample point on unit disk

		mvecSamples_UnitDisk.reserve(size);

		for (int j = 0; j < size; j++) {
			// map sample point to [-1, 1] X [-1,1]

			sp.x = 2.0f * mvecSamples_UnitSquare[j].x - 1.0f;
			sp.y = 2.0f * mvecSamples_UnitSquare[j].y - 1.0f;

			if (sp.x > -sp.y) {			// sectors 1 and 2
				if (sp.x > sp.y) {		// sector 1
					r = sp.x;
					phi = sp.y / sp.x;
				}
				else {					// sector 2
					r = sp.y;
					phi = 2 - sp.x / sp.y;
				}
			}
			else {						// sectors 3 and 4
				if (sp.x < sp.y) {		// sector 3
					r = -sp.x;
					phi = 4 + sp.y / sp.x;
				}
				else {					// sector 4
					r = -sp.y;
					if (sp.y != 0.0f)	// avoid division by zero at origin
						phi = 6 - sp.x / sp.y;
					else
						phi = 0.0f;
				}
			}

			phi *= PI_CONST / 4.0;

			mvecSamples_UnitDisk[j].x = r * std::cos(phi);
			mvecSamples_UnitDisk[j].y = r * std::sin(phi);
		}

	//	mvecSamples_UnitSquare.erase(mvecSamples_UnitSquare.begin(), mvecSamples_UnitSquare.end());
	}
	void SamplerBase::MapSamplesToHemiShpere(const float exp)
	{
		int size = mvecSamples_UnitSquare.size();
		mvecSamples_HemiShpere.reserve(mnSampleCount * mnSets);

		for (int j = 0; j < size; j++) {
			float cos_phi = std::cos(2.0f * PI_CONST * mvecSamples_UnitSquare[j].x);
			float sin_phi = std::sin(2.0f * PI_CONST * mvecSamples_UnitSquare[j].x);
			float cos_theta = std::pow((1.0f - mvecSamples_UnitSquare[j].y), 1.0f / (exp + 1.0f));
			float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
			float pu = sin_theta * cos_phi;
			float pv = sin_theta * sin_phi;
			float pw = cos_theta;
			mvecSamples_HemiShpere.push_back(Point3f(pu, pv, pw));
		}
	}
	void SamplerBase::MapSamplesToShpere(void)
	{
		float r1, r2;
		float x, y, z;
		float r, phi;

		mvecSamples_Shpere.reserve(mnSampleCount * mnSets);

		for (int j = 0; j < mnSampleCount * mnSets; j++) {
			r1 = mvecSamples_UnitSquare[j].x;
			r2 = mvecSamples_UnitSquare[j].y;
			z = 1.0f - 2.0f * r1;
			r = std::sqrt(1.0f - z * z);
			phi = TWO_PI_CONST * r2;
			x = r * std::cos(phi);
			y = r * std::sin(phi);
			mvecSamples_Shpere.push_back(Point3f(x, y, z));
		}
	}

	Point2f	SamplerBase::SampleFromUnitSquare(void)
	{
		if (mlCount % mnSampleCount == 0)  									// start of a new pixel
			mnJump = (Random::RandInt() % mnSets) * mnSampleCount;				// random index jump initialised to zero in constructor

		return (mvecSamples_UnitSquare[mnJump + mvecShuffledIndices[mnJump + mlCount++ % mnSampleCount]]);
	}
	Point2f SamplerBase::SampleFromUnitDisk(void)
	{
		if (mlCount % mnSampleCount == 0)  									// start of a new pixel
			mnJump = (Random::RandInt() % mnSets) * mnSampleCount;

		return (mvecSamples_UnitDisk[mnJump + mvecShuffledIndices[mnJump + mlCount++ % mnSampleCount]]);
	}
	Point3f SamplerBase::SampleFromHemishpere(void)
	{
		if (mlCount % mnSampleCount == 0)  									// start of a new pixel
			mnJump = (Random::RandInt() % mnSets) * mnSampleCount;

		return (mvecSamples_HemiShpere[mnJump + mvecShuffledIndices[mnJump + mlCount++ % mnSampleCount]]);
	}
	Point3f SamplerBase::SampleFromShpere(void)
	{
		if (mlCount % mnSampleCount == 0)  									// start of a new pixel
			mnJump = (Random::RandInt() % mnSets) * mnSampleCount;

		return (mvecSamples_Shpere[mnJump + mvecShuffledIndices[mnJump + mlCount++ % mnSampleCount]]);
	}

	Point2f SamplerBase::SampeFromOneSet(void)
	{
		return(mvecSamples_UnitSquare[mlCount++ % mnSampleCount]);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Concrete sampler sub classes go here...
	//
	// RegularSampler
	RegularSampler::RegularSampler(void)
		: SamplerBase()
	{
		
	}
	RegularSampler::RegularSampler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	RegularSampler::RegularSampler(const RegularSampler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}

	RegularSampler& RegularSampler::operator= (const RegularSampler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator=(rhs);

		return (*this);
	}
	RegularSampler::~RegularSampler(void)
	{

	}

	void* RegularSampler::Clone(void)
	{
		return (new RegularSampler(*this));
	}

	void RegularSampler::GenerateSamples(void)
	{
		int n = (int)std::sqrt((float)mnSampleCount);

		for (int j = 0; j < mnSets; j++) {
			for (int p = 0; p < n; p++) {
				for (int q = 0; q < n; q++) {
					mvecSamples_UnitSquare.push_back(Point2f((q + 0.5) / n, (p + 0.5) / n));
				}
			}
		}
	}

	// PureRandomSampler
	//
	PureRandomSampler::PureRandomSampler(void)
		: SamplerBase()
	{

	}
	PureRandomSampler::PureRandomSampler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	PureRandomSampler::PureRandomSampler(const PureRandomSampler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}
	PureRandomSampler& PureRandomSampler::operator=(const PureRandomSampler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator=(rhs);

		return (*this);
	}
	PureRandomSampler::~PureRandomSampler(void)
	{

	}

	void* PureRandomSampler::Clone(void)
	{
		return (new PureRandomSampler(*this));
	}

	void PureRandomSampler::GenerateSamples(void)
	{
		for (int p = 0; p < mnSets; p++) {
			for (int q = 0; q < mnSampleCount; q++) {
				mvecSamples_UnitSquare.push_back(Point2f(Random::RandFloat(), Random::RandFloat()));
			}
		}
	}

	// Jittered
	//
	JitteredSampler::JitteredSampler(void)
		: SamplerBase()
	{

	}
	JitteredSampler::JitteredSampler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	JitteredSampler::JitteredSampler(const int num, const int ns)
		: SamplerBase(num, ns)
	{
		this->GenerateSamples();
	}
	JitteredSampler::JitteredSampler(const JitteredSampler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}
	JitteredSampler& JitteredSampler::operator=(const JitteredSampler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator= (rhs);

		return (*this);
	}
	JitteredSampler::~JitteredSampler(void)
	{

	}

	void* JitteredSampler::Clone(void)
	{
		return (new JitteredSampler(*this));
	}

	void JitteredSampler::GenerateSamples(void)
	{
		int n = (int)std::sqrt((float)mnSampleCount);

		for (int p = 0; p < mnSets; p++) {
			for (int j = 0; j < n; j++) {
				for (int k = 0; k < n; k++) {
					Point2f sp((k + Random::RandFloat()) / n, (j + Random::RandFloat()) / n);
					mvecSamples_UnitSquare.push_back(sp);
				}
			}
		}
	}

	// NRooksSampler
	//
	NRooksSampler::NRooksSampler(void)
		: SamplerBase()
	{

	}
	NRooksSampler::NRooksSampler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	NRooksSampler::NRooksSampler(const int num, const int ns)
		: SamplerBase(num, ns)
	{
		this->GenerateSamples();
	}
	NRooksSampler::NRooksSampler(const NRooksSampler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}
	NRooksSampler& NRooksSampler::operator=(const NRooksSampler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator=(rhs);

		return (*this);
	}
	NRooksSampler::~NRooksSampler(void)
	{
		
	}

	void* NRooksSampler::Clone(void)
	{
		return (new NRooksSampler(*this));
	}

	void NRooksSampler::GenerateSamples(void)
	{
		for (int p = 0; p < mnSets; p++)
			for (int j = 0; j < mnSampleCount; j++) {
				Point2f sp((j + Random::RandFloat()) / mnSampleCount, (j + Random::RandFloat()) / mnSampleCount);
				mvecSamples_UnitSquare.push_back(sp);
			}

		SamplerBase::ShuffleXCoordinates();
		SamplerBase::ShuffleYCoordinates();
	}

	// MultiJitteredSampler
	//
	MultiJitteredSampler::MultiJitteredSampler(void)
		: SamplerBase()
	{

	}
	MultiJitteredSampler::MultiJitteredSampler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	MultiJitteredSampler::MultiJitteredSampler(const int num, const int ns)
		: SamplerBase(num, ns)
	{
		this->GenerateSamples();
	}
	MultiJitteredSampler::MultiJitteredSampler(const MultiJitteredSampler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}
	MultiJitteredSampler& MultiJitteredSampler::operator=(const MultiJitteredSampler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator=(rhs);

		return (*this);
	}
	MultiJitteredSampler::~MultiJitteredSampler(void)
	{

	}

	void* MultiJitteredSampler::Clone(void)
	{
		return (new MultiJitteredSampler(*this));
	}

	void MultiJitteredSampler::GenerateSamples(void)
	{
		// num_samples needs to be a perfect square
		int n = (int)std::sqrt((float)mnSampleCount);
		float subcell_width = 1.0f / ((float)mnSampleCount);

		// fill the samples array with dummy points to allow us to use the [ ] notation when we set the 
		// initial patterns
		Point2f fill_point;
		for (int j = 0; j < mnSampleCount * mnSets; j++)
			mvecSamples_UnitSquare.push_back(fill_point);

		// distribute points in the initial patterns
		for (int p = 0; p < mnSets; p++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					mvecSamples_UnitSquare[i * n + j + p * mnSampleCount].x = (i * n + j) * subcell_width + Random::RandFloat(0, subcell_width);
					mvecSamples_UnitSquare[i * n + j + p * mnSampleCount].y = (j * n + i) * subcell_width + Random::RandFloat(0, subcell_width);
				}
			}
		}

		// shuffle x coordinates
		for (int p = 0; p < mnSets; p++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					int k = Random::RandInt(j, n - 1);
					float t = mvecSamples_UnitSquare[i * n + j + p * mnSampleCount].x;
					mvecSamples_UnitSquare[i * n + j + p * mnSampleCount].x = mvecSamples_UnitSquare[i * n + k + p * mnSampleCount].x;
					mvecSamples_UnitSquare[i * n + k + p * mnSampleCount].x = t;
				}
			}
		}

		// shuffle y coordinates
		for (int p = 0; p < mnSets; p++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					int k = Random::RandInt(j, n - 1);
					float t = mvecSamples_UnitSquare[j * n + i + p * mnSampleCount].y;
					mvecSamples_UnitSquare[j * n + i + p * mnSampleCount].y = mvecSamples_UnitSquare[k * n + i + p * mnSampleCount].y;
					mvecSamples_UnitSquare[k * n + i + p * mnSampleCount].y = t;
				}
			}
		}
	}

	// HammersleySmapler
	//
	HammersleySmapler::HammersleySmapler(void)
		: SamplerBase()
	{

	}
	HammersleySmapler::HammersleySmapler(const int num)
		: SamplerBase(num)
	{
		this->GenerateSamples();
	}
	HammersleySmapler::HammersleySmapler(const HammersleySmapler& rhs)
		: SamplerBase(rhs)
	{
		this->GenerateSamples();
	}
	HammersleySmapler& HammersleySmapler::operator=(const HammersleySmapler& rhs)
	{
		if (this == &rhs) return (*this);

		SamplerBase::operator=(rhs);

		return (*this);
	}
	HammersleySmapler::~HammersleySmapler(void)
	{

	}

	void* HammersleySmapler::Clone(void)
	{
		return (new HammersleySmapler(*this));
	}

	void HammersleySmapler::GenerateSamples(void)
	{
		for (int p = 0; p < mnSets; p++) {
			for (int j = 0; j < mnSampleCount; j++) {
				Point2f pv((float)j / (float)mnSampleCount, phi(j));
				mvecSamples_UnitSquare.push_back(pv);
			}
		}
	}

	float HammersleySmapler::phi(int j)
	{
		float x = 0.0f;
		float f = 0.5f;

		while (j) {
			x += f * (float)(j % 2);
			j /= 2;
			f *= 0.5f;
		}

		return (x);
	}
}
