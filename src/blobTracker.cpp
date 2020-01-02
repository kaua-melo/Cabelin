

#include "BlobTracker.h"
#include "Blob.h"

BlobTracker::BlobTracker(int mxD, bool calculateVel, bool d)
{
    //if(debug)     cout<< "n blobs detected: " << cBlobs.size() <<endl;

    font.loadFont("verdana.ttf", 15); //(you need verdana.ttf inside your bin/data/  folder)

    currentMaxID = 0;
    maxDistance = mxD;

    debug = d;
    calcVel = calculateVel;
}

void BlobTracker::findBlobs(ofxCvGrayscaleImage &im, int minArea, int maxArea, int max_n_blobs, bool findHoles)
{
    // Find blobs
    contourFinder.findContours(im, minArea, maxArea, max_n_blobs, false);
    // findCountours(ofxCvGrayscaleImage &input,
    //  int minArea,
    //  int maxArea,
    //  int nConsidered, -> maximum number of blobs to be considered
    //  bool bFindHoles,
    //  bool bUseApproximation=true)

    // Clear our cBlobs vector
    cBlobs.clear();

    cBlobs_dict.clear();

    // Pushing each blob to our vector of Blobs
    for (int i = 0; i < contourFinder.nBlobs; i++)
    {
        cBlobs.push_back(Blob(contourFinder.blobs[i]));
    }

    // Set IDS
    setIDs();

    // Now that the blobs all have IDs, we populate our dictionary using their
    //  IDs as the key.
    for (int i = 0; i < cBlobs.size(); i++)
    {
        cBlobs_dict[cBlobs[i].ID] = cBlobs[i];
    }

    // Calculate blobs' velocities if desired
    if (calcVel)
    {
        calcVelocities();
    }

    // Update the previous frame blobs to the current frame
    updatePreviousBlobs();
}

void BlobTracker::setIDs()
{

    if (debug)
        cout << "n blobs detected: " << cBlobs.size() << endl;

    // THERE ARE 4 POSSIBILITIES:

    // 1. There are no blobs in the scene ---------------------------------------
    if (cBlobs.size() == 0)
    {
        if (debug)
            cout << "NO BLOBS IN THE SCENE!" << endl;

        currentMaxID = 0;
    }
    // --------------------------------------------------------------------------

    // 2. N blobs in the current frame > N blobs in the previous frame ----------
    else if (cBlobs.size() > pBlobs.size())
    {
        if (debug)
            cout << " c > p" << endl;

        // If there were no blobs in the previous frame.
        if (pBlobs.size() == 0)
        {
            if (debug)
                cout << " p = 0" << endl;

            // Go through each blob and add its ID
            for (int i = 0; i < cBlobs.size(); i++)
            {
                if (debug)
                    cout << "adding ID: " << currentMaxID << endl;

                cBlobs[i].ID = currentMaxID++;
            }
        }
        // If there were some blobs in the previous frame
        else
        {
            matchIDs_more_or_equal_than_previous();
        }
    }
    // --------------------------------------------------------------------------

    // 3. N blobs in the current frame == N blobs in the previous frame ---------
    else if (cBlobs.size() == pBlobs.size())
    {
        matchIDs_more_or_equal_than_previous();
    }
    // --------------------------------------------------------------------------

    // 4. N blobs in the current frame < N blobs in the previous frame ----------
    if (cBlobs.size() < pBlobs.size())
    {
        matchIDs_less_than_previous();
    }
    // --------------------------------------------------------------------------

    if (debug)
    {
        // Printing IDs
        for (int i = 0; i < cBlobs.size(); i++)
        {
            cout << "Current ID -> " << cBlobs[i].ID << endl;
        }
    }
}

void BlobTracker::matchIDs_less_than_previous()
{
    // Go through the blobs of the current frame
    for (int c_i = 0; c_i < cBlobs.size(); c_i++)
    {
        float minDistSoFar = maxDistance;
        int indexOfTheClosestBlob = -1;

        if (debug)
            cout << "For Loop c_i= " << c_i << endl;

        // Go through the blobs of the previous frame
        for (int p_i = 0; p_i < pBlobs.size(); p_i++)
        {
            if (debug)
                cout << "For Loop p_i= " << p_i << endl;

            // Check the distance from the specific blob on the current frame
            //   to the specific blob in the previous frame so we can know what's
            //   the closest one.
            float d = sqrt(pow(pBlobs[p_i].blob.centroid.x - cBlobs[c_i].blob.centroid.x, 2) +
                           pow(pBlobs[p_i].blob.centroid.y - cBlobs[c_i].blob.centroid.y, 2));

            // If some blob was close enough (d < minDistSoFar)
            if (d < minDistSoFar)
            {
                minDistSoFar = d;
                indexOfTheClosestBlob = p_i;

                if (debug)
                    cout << " MISSING DEBUG MESSAGE HERE! " << endl;
            }
        }

        // When indexOfTheClosestBlob == -1  it means that no blob close enough (d < minDistSoFar) was detected.
        if (indexOfTheClosestBlob != -1)
        {
            cBlobs[c_i].ID = pBlobs[indexOfTheClosestBlob].ID;

            // Set what was its previous position so we can calculate its velocity later
            cBlobs[c_i].p_pos.set(pBlobs[indexOfTheClosestBlob].blob.centroid.x,
                                  pBlobs[indexOfTheClosestBlob].blob.centroid.y);
        }
    }

    // Now let's Add an ID to the blobs in the current frame which were not close to any
    //  blobs in the previous frame. These blobs are probably new blobs into the scene.
    // Go through the blobs of the current frame
    for (int i = 0; i < cBlobs.size(); i++)
    {
        // In case the blobs wasn't matched with any previous blobs (its ID should be -1 in that case)
        if (cBlobs[i].ID == -1)
        {
            cBlobs[i].ID = currentMaxID++;
        }
    }
}

void BlobTracker::matchIDs_more_or_equal_than_previous()
{

    // Go through the blobs of the previous frame
    for (int p_i = 0; p_i < pBlobs.size(); p_i++)
    {
        float minDistSoFar = maxDistance;
        int indexOfTheClosestBlob = -1;

        if (debug)
            cout << "For Loop p_i= " << p_i << endl;

        // Go through the blobs of the current frame
        for (int c_i = 0; c_i < cBlobs.size(); c_i++)
        {
            if (debug)
                cout << "For Loop c_i= " << c_i << endl;

            // Check the distance from the specific blob on the previous frame
            //   to the specific blob in the current frame so we can know what's
            //   the closest one.
            float d = sqrt(pow(pBlobs[p_i].blob.centroid.x - cBlobs[c_i].blob.centroid.x, 2) +
                           pow(pBlobs[p_i].blob.centroid.y - cBlobs[c_i].blob.centroid.y, 2));

            // If some blob was close enough (d < minDistSoFar).
            //  indexOfTheClosestBlob == -1  means that no blob close enough (d < minDistSoFar) was detected.
            if (d < minDistSoFar)
            {
                minDistSoFar = d;
                indexOfTheClosestBlob = c_i;
                if (debug)
                    cout << " minDistSoFar = " << minDistSoFar << "  ID OfTheClosestBlob: " << pBlobs[p_i].ID << endl;
            }
        }

        // When indexOfTheClosestBlob == -1  it means that no blob close enough (d < minDistSoFar) was detected.
        if (indexOfTheClosestBlob != -1)
        {
            cBlobs[indexOfTheClosestBlob].ID = pBlobs[p_i].ID;

            // Set what was its previous position so we can calculate its velocity later
            cBlobs[indexOfTheClosestBlob].p_pos.set(pBlobs[p_i].blob.centroid.x,
                                                    pBlobs[p_i].blob.centroid.y);
        }
    }

    // Now let's Add the an ID to the blobs in the current frame which were not close to any
    //  blobs in the previous frame. These blobs are probably new blobs into the scene.
    // Go through the blobs of the current frame
    for (int i = 0; i < cBlobs.size(); i++)
    {
        // In case the blobs wasn't matched with any previous blobs (its ID should be -1 in that case)
        if (cBlobs[i].ID == -1)
        {
            cBlobs[i].ID = currentMaxID++;
        }
    }
}

void BlobTracker::updatePreviousBlobs()
{
    pBlobs.clear();
    pBlobs = cBlobs;
}

void BlobTracker::drawContainers(int x, int y)
{

    map<int, Blob>::iterator it;
    for (it = cBlobs_dict.begin(); it != cBlobs_dict.end(); it++)
    {
        it->second.drawContainer(x, y, font);
    }
}

void BlobTracker::drawVelocities(int x, int y)
{

    map<int, Blob>::iterator it;
    for (it = cBlobs_dict.begin(); it != cBlobs_dict.end(); it++)
    {
        it->second.drawVelocity(x, y);
    }
}

void BlobTracker::calcVelocities()
{

    map<int, Blob>::iterator it;
    for (it = cBlobs_dict.begin(); it != cBlobs_dict.end(); it++)
    {
        it->second.calcVel();
    }
}

void BlobTracker::setMaxDistance(int md)
{
    maxDistance = md;
}
