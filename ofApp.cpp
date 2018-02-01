#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofBackground(255);
	ofSetWindowTitle("Insta");


	for (int i = 0; i < 32 + 12; i++) {

		unique_ptr<Particle> particle(new Particle());
		this->particles.push_back(std::move(particle));
	}

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	this->box2d.createBounds();
	this->box2d.setFPS(60);
	this->box2d.registerGrabbing();

	this->fbo.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {

	for (int i = this->polygons.size() - 1; i >= 0; i--) {
		
		this->polygons_life[i] -= 1;

		if (this->polygons_life[i] < 20) {

			this->polygons[i].get()->destroy();
			this->polygons.erase(this->polygons.begin() + i);
			this->polygons_life.erase(this->polygons_life.begin() + i);
			this->polygons_color.erase(this->polygons_color.begin() + i);
		}
	}


	this->fbo.begin();
	ofClear(0);

	for(int i = 0; i < this->particles.size(); i++){

		this->particles[i]->update();

		vector<ofVec2f> near_points;
		for (int j = i; j < this->particles.size(); j++) {

			float distance = this->particles[i]->get_location().distance(this->particles[j]->get_location());

			if (distance < 50) {

				ofSetColor(this->particles[i]->get_body_color(), 128);
				ofDrawLine(this->particles[i]->get_location(), this->particles[j]->get_location());
				if (distance < 35) {
					near_points.push_back(this->particles[j]->get_location());
				}
			}
		}

		if (near_points.size() >= 3) {

			shared_ptr<ofxBox2dPolygon> polygon = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
			for (int n_index = 0; n_index < near_points.size(); n_index++) {
				polygon.get()->addVertex(near_points[n_index]);
			}

			polygon.get()->setPhysics(1.0, 0.7, 0.3);
			polygon.get()->create(box2d.getWorld());
			
			this->polygons.push_back(polygon);
			this->polygons_life.push_back(255);
			ofColor c;
			c.setHsb(ofRandom(255), 255, 255);
			this->polygons_color.push_back(c);
		}
	}

	this->box2d.update();

	for (int i = 0; i < this->polygons.size(); i++) {

		ofSetColor(this->polygons_color[i], this->polygons_life[i]);
		this->polygons[i].get()->draw();
	}

	for (int i = 0; i < this->particles.size(); i++) {

		this->particles[i]->draw();
	}

	this->fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->fbo.draw(0, 0);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}