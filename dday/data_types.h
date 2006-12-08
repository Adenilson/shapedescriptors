#ifndef __BLOB_DATA_TYPES__
#define __BLOB_DATA_TYPES__

#include <complex>


/** Square of number
 *
 * @param i A number to be squared.
 *
 * @return Squared number.
 *
 */
template <typename T>
inline T square(T i)
{
	return i * i;
}


/** Blobs descriptors features
 *
 * TODO: Doxygen comments.
 */
struct blob_features {
	/** Some feature descriptors of blobs */
	float area, perimeter;
	std::complex<float> centroid;
	/** minimum coordinate values */
	float min_x, min_y;
	/** maximum coordinate values */
	float max_x, max_y;
	/** Bounding box diagonal */
	float rectangularity;

	/** Function to calculate centroid */
	void calc_centroid() {
		centroid.real() = (min_x + max_x) / 2;
		centroid.imag() = (min_y + max_y) / 2;
	}

	/** Diagonal of given blob bouding box
	 *
	 * @param blob_features A struct with blob extracted features.
	 *
	 * @return The diagonal of the bounding box of blob.
	 *
	 */
	void calc_rectangularity() {

		float diagonal;
		diagonal = square(min_x - max_x) +
			square(min_y - max_y);
		diagonal = sqrt(diagonal);
		rectangularity = area / diagonal;
	}

	/** Overloaded operator */
	blob_features &operator=(const blob_features &obj) {
		rectangularity = obj.rectangularity;
		area = obj.area;
		perimeter = obj.perimeter;
		min_x = obj.min_x;
		min_y = obj.min_y;
		max_x = obj.max_x;
		max_y = obj.max_y;
		centroid = obj.centroid;
	}
};

/** Structure to hold filtered blobs.
 * TODO: Add doxygen comments in struct methods.
 */
struct blob_result {
	/** holds blob descriptors vector */
	blob_features *blobs;

	/** number of blobs within vector */
	int blob_count;

	/** Default empty constructor */
	blob_result(void): blob_count(0), blobs(NULL) { }

	/** Allocate memory space for blobs */
	int allocate(int length) {
		blob_count = length;
		blobs = new blob_features[blob_count];
		if (!blobs) {
			blob_count = 0;
			blobs = NULL;
		}
		return blob_count;
	}

	/** Constructor to allocate blob object vector */
	blob_result(int num_blobs) {
		allocate(num_blobs);
	}

	blob_result & operator=(const blob_result &obj) {
		if (allocate(obj.blob_count)) {
			for (int i = 0; i < blob_count; ++i)
				blobs[i] = obj.blobs[i];
		}
		return *this;
	}

	/** Destructor, free up resources
	 * TODO: set pointer to NULL
	 */
	~blob_result() {
		if (blobs)
			delete [] blobs;
	}

};


#endif
