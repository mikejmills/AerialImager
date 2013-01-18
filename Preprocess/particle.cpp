
#include "particle.h"


//
// Particle Data structure
typedef struct {
    short int x, y;
    float weight;
    CvScalar pix;
} Particle;

//
// Sample Data Structure
typedef struct {
    vector<Particle *> *X;
    int count;
    CvScalar average;
} Sample;

//
// Sorting routine for the particles in a vector list
bool Particle_Sort_Pred(const Particle *x1, const Particle *x2)
{
    return x1->weight > x2->weight;
}

//
// Return a random pixel from within the window around the center cx,cy pixel
// img          - image
// cx, cy       - center of window
// winx, winy   - Width and height of window
// x, y         - resulting pixel coordinate
void random_pixel(IplImage *img, int winx, int winy, int cx, int cy, int *x, int *y)
{
    int xt = 0, yt = 0;
    
    // Repeat this random sampling until we have a valid pixel
    do {
        
        xt = rand() % winx;
        yt = rand() % winy;
        
        if ((rand() % 2) == 0) xt *= -1;
        if ((rand() % 2) == 0) yt *= -1;
        
        xt = cx + xt;
        yt = cy + yt;
        
        
    } while( ((xt < 0) || (xt > img->width - 1)) || ((yt < 0) || (yt > img->height - 1)) );
    
    
    *x = xt;
    *y = yt;
    // printf("a %d %d\n", *x, *y); 
}


//
// Generate Weight
// Generates a weight based on the difference between the given pixel "pix" and 
// a random pixel choosen from within a window winx, winy around the pix coord x, y. 
// This procedure is repeated sample_size times and all adds up to the returned weight
int gen_smooth_area_weight(IplImage *img, CvScalar *pix, int x, int y, int winx, int winy, int sample_size)
{
    int xt, yt, diff = 0;
    
    CvScalar pix1;
    
    
    for (int i = 0; i < sample_size; i++) {
        xt = x;
        yt = y;
        
        random_pixel(img, winx, winy, x, y, &xt, &yt);
        pix1 = cvGet2D(img, yt, xt);
        
        diff += ( fabs(pix->val[0] - pix1.val[0]) + fabs(pix->val[1] - pix1.val[1]) + fabs(pix->val[2] - pix1.val[2]) );
        
    }
    
    return diff;
}


//
// Generate the a sample distribution particles for entire picture
// Return the Sample struct
Sample *Generate_Sample(IplImage *img, int m)
{
    Particle *p;
    CvScalar average;
    
    average.val[0] = 0;
    average.val[1] = 0;    
    average.val[2] = 0;
    
    
    Sample *sample = new Sample;
    
    if (!sample) return NULL;
    
    sample->X = new vector<Particle *>;
    
    if (!sample->X) {
        delete sample;
        return NULL;
    }
    
    
    for (int i = 0; i < m; i++) {
        
        // Create new particle
        p = new Particle;
        
        if (!p) continue;
        
        // Random coordinates
        p->x = rand() % img->width  ;
        p->y = rand() % img->height ;
        
        // save pixel pointer
        p->pix = cvGet2D(img, p->y, p->x);
        
        // Init weight to 0.5 or equal probability of all particles
        p->weight = 0.5;
        
        
        // Save for averaging
        average.val[0] += p->pix.val[0];
        average.val[1] += p->pix.val[1];
        average.val[2] += p->pix.val[2];  
        
        sample->X->push_back(p);
        
    }
    
    // Calculate average of this sample
    sample->average.val[0] = (float) average.val[0] / m;
    sample->average.val[1] = (float) average.val[1] / m;
    sample->average.val[2] = (float) average.val[2] / m;
    
    printf("Average %f %f %f\n", sample->average.val[0], sample->average.val[1], sample->average.val[2] );
    
    return sample;
}


//
// Add the particles in list to the probability image img
void add_resample(IplImage *img, vector<Particle *> *list, int size)
{
    CvScalar s;
    s.val[0] = 255;
    
    
    vector<Particle *>::iterator l;    
    for (l = list->begin(); l < list->begin() + size; l++) {
        cvSet2D(img, (*l)->y, (*l)->x, s);
    }
    
    return;
}


//
// Generate the probability image using the particle sampling
IplImage *Probability_Image(IplImage *img, float top_percent, int sample_size, int iterations)
{
    Particle *part;
    Sample *sample;
    vector<Particle *>::iterator i;
    int mat_start, resample_size;
    
    IplImage *prob_img = cvCreateImage(cvGetSize(img), img->depth, 1);
    
    if (!prob_img) return NULL;
    
    cvZero(prob_img);
    
    top_percent = top_percent/100;
    resample_size = sample_size * top_percent;
    mat_start = 0;
    
    //printf("resample %d\n", resample_size);
    
   
    
    for (int j = 0; j < iterations; j++) {
        
        sample = Generate_Sample(img, sample_size);
        if (!sample) return NULL;
        
        for (i = sample->X->begin(); i < sample->X->end(); i++) {
            part = *i;
            if (!part) continue;
            
            // Provide partical weight based on the difference from average
            part->weight = gen_smooth_area_weight(img, &(part->pix), part->x, part->y, 60, 60, 20);
            
        }   
        
        // Sort by weight
        std::sort( sample->X->begin(), sample->X->end(), &Particle_Sort_Pred );
        
        // Save the top_percent of particals
        add_resample(prob_img, sample->X, resample_size);
        
        // Free all other entries
        for (i = sample->X->begin(); i < sample->X->end(); i++) {
            delete *i;
        }
        
        delete sample->X;
        delete sample;
        sample = NULL;
    }
    
    if (sample) delete sample;
    
    return prob_img;
}


//
// Returns an array of targets in CvRect
vector<CvRect> *Get_Targets(IplImage *prob_img)
{
    double area;
    CvSeq* contours = 0;
    int count = 0;
    CvRect rec;
    
    // Allocate the target space
    vector<CvRect> *targets  = new vector<CvRect>;
    
    if (!targets) return NULL;
    
    CvMemStorage* storage = cvCreateMemStorage(0);
    
    cvFindContours( prob_img, storage, &contours, sizeof(CvContour),
                   CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
    
    for (CvSeq *c=contours; c != NULL; c=c->h_next) {
        
                
        area = abs(cvContourArea( c, CV_WHOLE_SEQ));
        
        // Remove any contours with area not between these values
        // these values are derived from experimentation
        if ((area > 1000) && (area <  13000)) {
            rec = cvBoundingRect(c, 0);
            
            // Remove any large bounding boxes, targets are generally 
            // square areas
            if (rec.width > 200 || rec.height > 200)
                continue;
            
            targets->push_back( rec );
            count++;
        }
        
        // Too many targets detected return empty list
        if (count >= MAX_TARGETS) {
            printf("Too many targets Detected %d\n", count);
            targets->clear();
            return targets;
        }
        
    }
    
    return targets;
}


// 
// Find the targets and add them to the TileFile
TileFile target_find(TileFile tf, IplImage *img)
{
    unsigned char target_headers[MAX_TARGETS * IMAGE_TARGETS_HEADER];
    
    srand( time(NULL) );
    
    long int offset;
    vector<CvRect> *targets;
    CvRect rec;
    
    // Get the probability image and some temp work space
    IplImage *prob_img = Probability_Image(img, 2, 250000, 3);
    if (!prob_img) return tf;
    
    IplImage *tmp = cvCloneImage(prob_img);
    
    if (!tmp) {
        cvReleaseImage(&prob_img);
        return tf;
    }
    
    // Smooth the discrete Probability image into a more continous form
    cvSmooth(prob_img, tmp, CV_BLUR , 15, 15, 0);
    
    // Remove the low density areas
    cvThreshold(tmp, prob_img, 25, 255, CV_THRESH_BINARY);
    
    targets = Get_Targets(prob_img);
    
    if (!targets) {
        cvReleaseImage(&prob_img);
        cvReleaseImage(&tmp);
        return tf;
    }
    
    for (unsigned int i= 0; i < targets->size(); i++) {
        offset = (long int)target_headers  + (IMAGE_TARGETS_HEADER * i);
        
        rec = targets->at(i);
        
        // Set the target Data
        INT(offset, IMAGE_TARGETS_center_x) = (rec.x + rec.width / 2);
        INT(offset, IMAGE_TARGETS_center_y) = (rec.y + rec.height / 2);
        INT(offset, IMAGE_TARGETS_width)    = rec.width;
        INT(offset, IMAGE_TARGETS_height)   = rec.height;

    }
    
    printf("Found %d\n", targets->size() );
    
    // Add targets to the TileFile
    tf = Add_Targets(tf, target_headers, targets->size());
    
    if (!tf) {
        fprintf(stderr, "Error adding targets\n");
    }
    
    // Release temp space
    cvReleaseImage(&tmp);
    cvReleaseImage(&prob_img);
    
    targets->clear();
    delete targets;
    
    
    return tf;
}




