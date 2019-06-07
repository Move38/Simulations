/*  Forest Fire
    by Jonathan Bobrow, Move38
    06/04/2019

    Press a Tree to control burn (no spreading)
    Long Press a Tree to strike lightning, it will ignite and then spread

    Plots of land grow trees (quicker around other Trees)... Each plot has
    up to 6 Trees
    The opportunity for a Tree to grow happens 3 times per second and fire
    spreads at this rate as well.
    The forest shares a heartbeat, so trees grow at the same time
*/

// firefly style sync

enum LandType {
  SOIL,
  TREE,
  CTRL_FIRE,
  WILD_FIRE
};

byte myLandType[6] = {SOIL, SOIL, SOIL, SOIL, SOIL, SOIL};
byte myTreeLevel[6] = {0, 0, 0, 0, 0, 0};
#define MAX_TREE_LEVEL 100
#define TREE_GROWTH_STEP 5

#define CTRL_FIRE_BURN_STEP 30
#define WILD_FIRE_BURN_STEP 20

#define LIGHT_DURATION 600
Timer lighteningStrikeTimer;
bool didLighteningStrike = false;


#define STEP_DURATION 200
Timer stepTimer;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  if (buttonLongPressed()) {//GROW TREES
    if (!haveTrees() && !haveFire()) {
      FOREACH_FACE(f) {
        myLandType[f] = TREE;
        myTreeLevel[f] = 10;
      }
    }
  }

  if (buttonSingleClicked()) {//CONTROLLED FIRE
    if (haveTrees() && !haveFire()) {
      FOREACH_FACE(f) {
        setLandTypeOnFace(CTRL_FIRE, f);
      }
    }
  }

  if (buttonDoubleClicked()) {//LIGHTNING
    FOREACH_FACE(f) {
      if (myLandType[f] == TREE) {
        myLandType[f] = WILD_FIRE;
      }
    }
  }

  if (stepTimer.isExpired()) {

    FOREACH_FACE(f) {
      if (myLandType[f] == WILD_FIRE) {

        //BURN
        if (myTreeLevel[f] <= WILD_FIRE_BURN_STEP) {//last step of this fire
          myTreeLevel[f] = 0;
          myLandType[f] = SOIL;
        } else {//regular burn
          myTreeLevel[f] -= WILD_FIRE_BURN_STEP;
        }

      } else if (myLandType[f] == CTRL_FIRE) {

        //BURN
        if (myTreeLevel[f] <= CTRL_FIRE_BURN_STEP) {//this is the last step of this fire
          myTreeLevel[f] = 0;
          myLandType[f] = SOIL;
        } else {//regular burn down
          myTreeLevel[f] -= CTRL_FIRE_BURN_STEP;
        }

        //check for neighboring wildfire just in case
        if (myLandType[f] == CTRL_FIRE) { //checking just in case we transitioned to SOIL
          if (hasNeighboringWildFire(f)) {//one of my neighbors is wildfire. I will become wildfire too
            myLandType[f] = WILD_FIRE;
          }
        }

      } else if (myLandType[f] == TREE) {

        //grow
        if (myTreeLevel[f] < MAX_TREE_LEVEL) {//this tree can still grow
          myTreeLevel[f] += TREE_GROWTH_STEP;
        }

        //check for wildfire
        if (hasNeighboringWildFire(f)) {
          if (random(1) == 0) {
            myLandType[f] = WILD_FIRE;
          }
        }

      } else if (myLandType[f] == SOIL) {

        //SPROUT
        if (hasNeighboringTree(f)) {//if I have neighboring trees, chances are higher
          if (random(7) == 0) {
            myLandType[f] = TREE;
            myTreeLevel[f] = 10;
          }
        } else {//when alone, chances are much smaller
          if (random(20) == 0) {
            myLandType[f] = TREE;
            myTreeLevel[f] = 10;
          }
        }

      }

    }

    // reset the step timer
    stepTimer.set(STEP_DURATION);
  }

  // DISPLAY STATES
  FOREACH_FACE(f) {
    switch (myLandType[f]) {
      case SOIL:      setColorOnFace(OFF, f);    break;
      case TREE:      setColorOnFace(dim(GREEN, 155 + myTreeLevel[f]), f);   break;
      case CTRL_FIRE: setColorOnFace(dim(ORANGE, 155 + myTreeLevel[f]), f);   break;
      case WILD_FIRE: setColorOnFace(dim(RED, 155 + myTreeLevel[f]), f);   break;
    }
  }

  // SHARE MY STATE
  FOREACH_FACE(f) {
    setValueSentOnFace(myLandType[f], f);
  }
}

bool hasNeighboringTree(byte face) {
  bool treeNeighbors = false;

  if (getLandTypeToMyLeft(face) == TREE) {
    treeNeighbors = true;
  }

  if (getLandTypeToMyRight(face) == TREE) {
    treeNeighbors = true;
  }

  if (!isValueReceivedOnFaceExpired(face)) {//neighbor!
    byte neighbor = getLastValueReceivedOnFace(face);
    if (neighbor == TREE) {
      treeNeighbors = true;
    }
  }

  return treeNeighbors;
}

bool hasNeighboringWildFire(byte face) {
  bool wildfireNeighbors = false;

  if (getLandTypeToMyLeft(face) == WILD_FIRE) {
    wildfireNeighbors = true;
  }

  if (getLandTypeToMyRight(face) == WILD_FIRE) {
    wildfireNeighbors = true;
  }

  if (!isValueReceivedOnFaceExpired(face)) {//neighbor!
    byte neighbor = getLastValueReceivedOnFace(face);
    if (neighbor == WILD_FIRE) {
      wildfireNeighbors = true;
    }
  }

  return wildfireNeighbors;
}

byte getLandTypeToMyLeft(byte face) {
  if (face > 0) {
    return myLandType[face - 1];
  }
  else {
    return myLandType[5];
  }
}

byte getLandTypeToMyRight(byte face) {
  if (face < 5) {
    return myLandType[face + 1];
  }
  else {
    return myLandType[0];
  }
}

void setLandTypeOnFace( byte type, byte face) {
  myLandType[face] = type;
};

bool haveTrees() {
  bool treeFound = false;
  FOREACH_FACE(f) {
    if (myLandType[f] == TREE) {
      treeFound = true;
    }
  }

  return treeFound;
}

bool haveFire() {
  bool fireFound = false;
  FOREACH_FACE(f) {
    if (myLandType[f] == WILD_FIRE || myLandType[f] == CTRL_FIRE) {
      fireFound = true;
    }
  }

  return fireFound;
}



