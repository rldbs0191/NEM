#include "node.h"

Node::Node() {
	REGION = 0;
	DIM = 0;
	FLUX = nullptr;
	BETA = nullptr;
	WIDTH = nullptr;
	NEIGHBOR = nullptr;
	Q = nullptr;
	C = nullptr;
	OUT_CURRENT = nullptr;
	INCOM_CURRENT = nullptr;
	DL = nullptr;
	BOUNDARY = nullptr;
}

Node::Node( int region, int dim) {
	REGION = region;
	DIM = dim;
	FLUX = nullptr;
	BETA = nullptr;
	WIDTH = nullptr;
	NEIGHBOR = nullptr;
	Q = nullptr;
	C = nullptr;
	OUT_CURRENT = nullptr;
	INCOM_CURRENT = nullptr;
	DL = nullptr;
	BOUNDARY = nullptr;
}

void Node::updateTransverseLeakage( int dim, int group ) {

}

void Node::update1Dflux() {

}

void Node::updateAverageFlux() {

}

void Node::updateOutgoingCurrent() {

}