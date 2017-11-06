// RU ZHANG 100421055 //
// Gurmukh Sandhu 100624009 //
// Alvin Aniwa 100603501 //
// Kyle Disante 100617178 //

#pragma once
#include "GameObject.h"
#include "AnimationMath.h"

#include <vector>
#include <iostream>
#include <string>
#include <math.h>

#include <GLUT/freeglut.h>
#include <TTK/GraphicsUtils.h>
#include <TTK/Camera.h>
#include <PointHandle.h>

template <typename T>
struct SpeedControlTableRow
{
	int segment;
	float tValue;
	float arcLength;
	float arcLengthNormalized;
	T sampleValue;
};

template <typename T>
class KeyframeController
{
private:
	unsigned int m_pCurrentKeyframe;
	unsigned int m_pNextKeyframe;
	std::vector<T> m_pKeys;
	float m_pKeyLocalTime;

	std::vector< SpeedControlTableRow<T> > m_pSpeedControlLookUpTable;

public:
	KeyframeController()
		: m_pCurrentKeyframe(0),
		m_pNextKeyframe(1),
		m_pKeyLocalTime(0.0f),
		paused(false),
		loops(true),
		doesSpeedControl(false),
		curvePlaySpeed(1.0f)
	{

	}

	void calculateLookupTable(int numSamples)
	{
		if (m_pKeys.size() > 1) // Make sure we have enough points
		{
			m_pSpeedControlLookUpTable.clear(); // clear existing table

			float timeStep = 1.0f / numSamples; // This controls the interval at which we will sample the segment

			// Create table and compute segment, t value and sample columns of table

			for (unsigned int i = 0; i < m_pKeys.size() - 1; i++) // loop through each segment
			{
				for (float j = 0.0f; j <= 1.0f; j += timeStep) // iterate through each sample on the current segment
				{
					// Todo:
					// Create a new SpeedControlTableRow and fill it in with the appropriate data
					SpeedControlTableRow<T> row;
					row.segment = i;
					row.tValue = j;
					row.sampleValue = doCatmull(j); // For this lab, we'll use lerp. But this exact algorithm works for or bezier too.

					m_pSpeedControlLookUpTable.push_back(row);
				}
			}

			// Calculate arc length column of table
			int numEntries = m_pSpeedControlLookUpTable.size();
			std::cout << numEntries << std::endl;


			if (numEntries == 0) // if you did the above loop correctly, this shouldnt happen
			{
				std::cout << "Failed to create look up table. " << std::endl;
				return;
			}

			// Initialize first row of table
			// Remember the struct has no ctor so we need to make sure to set everything manually
			// Note: the slides refer "arcLength" as "distance on curve"
			m_pSpeedControlLookUpTable[0].arcLength = 0.0f;
			m_pSpeedControlLookUpTable[0].arcLengthNormalized = 0.0f;


			// Loop through each point in the table and calculate the distance from the beginning of the path
			for (int i = 1; i < numEntries; i++) 
			{
				float distance = glm::length(m_pSpeedControlLookUpTable[i].sampleValue - m_pSpeedControlLookUpTable[i - 1].sampleValue);
				
				m_pSpeedControlLookUpTable[i].arcLength = distance + m_pSpeedControlLookUpTable[i - 1].arcLength;
			}

			// Normalize the curve
			// This means 0 will be at the start of the path, and 1 will be at the end of the entire path
			float totalCurveLength = m_pSpeedControlLookUpTable[numEntries - 1].arcLength; // length of the path = distance the last sample is from the beginning

			// Normalize each sample
			// Loop through each entry in the table
			// Set "ArcLengthNormalized" to sample's distance on curve divided by total length of curve
			for (int i = 1; i < numEntries; i++)
			{
				m_pSpeedControlLookUpTable[i].arcLengthNormalized = m_pSpeedControlLookUpTable[i].arcLength / totalCurveLength;
			}
		}
	}

	T speedControlledUpdate(float dt)
	{
		// key local time is now from start of curve to end of curve, instead of per segment

		m_pKeyLocalTime += (dt / curvePlaySpeed); // control playback speed

		updateSegmentIndices();

		// look up values from table

		// Loop through each row in the table
		for (unsigned int i = 1; i < m_pSpeedControlLookUpTable.size(); i++)
		{
			// Find the first sample who's distance is >= m_pKeyLocalTime
			if (m_pSpeedControlLookUpTable[i].arcLengthNormalized >= m_pKeyLocalTime)
			{
				// calculate t value

				// float arc0 = previous sample's normalized distance
				float arc0 = m_pSpeedControlLookUpTable[i - 1].arcLengthNormalized;
				//float arc1 = m_pSpeedControlLookUpTable[i].arcLengthNormalized; // current sample's normalized distance
				float arc1 = m_pSpeedControlLookUpTable[i].arcLengthNormalized;
				//float tVal = Math::invLerp(m_pKeyLocalTime, arc0, arc1); // "inverse lerp" i.e. given 3 points, solve the tValue
				float tVal = Math::invCatmull(m_pKeyLocalTime, arc0, arc1);
				
				// calculate intermediate table
				//T sample0 = m_pSpeedControlLookUpTable[i - 2].sampleValue;
				//T sample1 = m_pSpeedControlLookUpTable[i - 1].sampleValue;
				//T sample0 = m_pSpeedControlLookUpTable[i - 1].sampleValue;
				//T sample0 = m_pSpeedControlLookUpTable[i - 1].sampleValue;


				//return Math::lerp(sample0, sample1, tVal);
				return doCatmull(tVal);
			}
		}

		return T(); // if lookup table is set up correctly, this should never trigger
	}	

	T update(float dt)
	{
		if (m_pKeys.size() < 2)
			return T();	

		if (doesSpeedControl)
			return speedControlledUpdate(dt);
		else
			return noSpeedControlUpdate(dt);
	}

	void updateSegmentIndices()
	{
		if (m_pKeyLocalTime >= 1.0f)
		{
			m_pKeyLocalTime = 0.0f;
			m_pCurrentKeyframe++;
			m_pNextKeyframe++;

			// If we're at the end of the animation, jump back to beginning
			// Note: you can add additional logic here to handle end of animation behaviour
			// such as: ping-ponging (playing in reverse back to beginning), closed loop, etc.
			if (m_pNextKeyframe >= m_pKeys.size())
			{
				m_pCurrentKeyframe = 0;
				m_pNextKeyframe = 1;
			}
		}
	}

	T noSpeedControlUpdate(float dt)
	{
		m_pKeyLocalTime += dt;

		updateSegmentIndices();

		T p0 = m_pKeys[m_pCurrentKeyframe];
		T p1 = m_pKeys[m_pNextKeyframe];

		return Math::lerp<T>(p0, p1, m_pKeyLocalTime);
	}

	void addKey(T key)
	{
		m_pKeys.push_back(key);
	}

	void setKey(unsigned int idx, T key)
	{
		if (idx >= m_pKeys.size())
			return;
		m_pKeys[idx] = key;
	}

	glm::vec3 doCatmull(float t)
	{
		// Not enough points, return
		if (pointHandles.size() < 4)
			return glm::vec3();
	
		glm::vec3 p0, p1, p2, p3;
	
		// TODO: currently the spline is calculated using hard coded indices
		// Implement the necessary logic to make this work for all points
		if ((m_pNextKeyframe + 2) <= pointHandles.size() & m_pCurrentKeyframe >= 1)
		{
			p0 = pointHandles[m_pCurrentKeyframe - 1].position;
			p1 = pointHandles[m_pCurrentKeyframe].position;
			p2 = pointHandles[m_pNextKeyframe].position;
			p3 = pointHandles[m_pNextKeyframe + 1].position;
		}
		else
		{
			return glm::vec3();
		}
	
		return catmullUMP(p0, p1, p2, p3, t);
	}
	
	void drawCatmull()
	{
		if (pointHandles.size() < 4)
			return;
	
		for (int f = 1; f <= (pointHandles.size() - 3); f++)
		{
			glm::vec3 p0, p1, p2, p3;
			//std::cout << pointHandles.size() << " ; " << f << " ; " << m_pCurrentKeyframe << " ; " << m_pNextKeyframe << std::endl;
			// TODO: This is hard coded to draw the first segment
			// Implement the necessary logic to draw the entire path
			p0 = pointHandles[f - 1].position;
			p1 = pointHandles[f].position;
			p2 = pointHandles[f + 1].position;
			p3 = pointHandles[f + 2].position;
	
			// This loop samples the curve at the specified step rate and
			// draws lines between the samples. Try playing around with the step value
			// and observe what happens to the curve drawn.
			float step = 0.1;
			for (float i = 0.0f; i < 1.0f;)
			{
				// Perform the interpolation with two samples
				glm::vec3 a = Math::catmull(p0, p1, p2, p3, i);
				glm::vec3 b = Math::catmull(p0, p1, p2, p3, i += step);
	
				// Create a line using the two samples
				TTK::Graphics::DrawLine(a, b, 5.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	
				// Draw the samples as a visualization (not necessary)
				TTK::Graphics::DrawPoint(a, 5.0f, glm::vec4(1.0f));
				TTK::Graphics::DrawPoint(b, 5.0f, glm::vec4(1.0f));
			}
		}
	}

	// Returns a point between p0 and p1 using catmull rom interpolation
	glm::vec3 catmullUMP(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
	{
		float t2 = t*t;
		float t3 = t2 * t;
		glm::vec4 U(t3, t2, t, 1.0f);
	
		// Note: you should store the M matrix somewhere so you
		// do not need to re-create it every time this function is called
		glm::mat4 M;
		M = glm::row(M, 0, glm::vec4(-1.0f, 3.0f, -3.0f, 1.0f));
		M = glm::row(M, 1, glm::vec4(2.0f, -5.0f, 4.0f, -1.0f));
		M = glm::row(M, 2, glm::vec4(-1.0f, 0.0f, 1.0f, 0.0f));
		M = glm::row(M, 3, glm::vec4(0.0f, 2.0f, 0.0f, 0.0f));
	
		glm::mat4 P;
		P = glm::row(P, 0, glm::vec4(p0, 0.0f));
		P = glm::row(P, 1, glm::vec4(p1, 0.0f));
		P = glm::row(P, 2, glm::vec4(p2, 0.0f));
		P = glm::row(P, 3, glm::vec4(p3, 0.0f));
	
		return 0.5f * U * M * P;
	}

	float curvePlaySpeed;
	bool paused;
	bool loops;
	bool doesSpeedControl;
};