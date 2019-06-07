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

  if (buttonDoubleClicked()) {
    FOREACH_FACE(f) {
      setLandTypeOnFace(TREE, f);
    }
  }

  if (buttonSingleClicked()) {
    FOREACH_FACE(f) {
      setLandTypeOnFace(CTRL_FIRE, f);
    }
  }

  if (buttonLongPressed()) {
    didLighteningStrike = true;
    lighteningStrikeTimer.set(LIGHT_DURATION);
  }

  if (didLighteningStrike && lighteningStrikeTimer.isExpired()) {
    FOREACH_FACE(f) {
      setLandTypeOnFace(WILD_FIRE, f);
    }

    didLighteningStrike = false;
  }

  if (stepTimer.isExpired()) {
    // ready to evaluate world

    // evaluate the world one face at a time :)
    FOREACH_FACE(f) {

      // burn out after we have been fire
      if (myLandType[f] == WILD_FIRE || myLandType[f] == CTRL_FIRE) {
        myLandType[f] = SOIL;
      }

      byte numTrees = 0;
      // check neighboring Blink if present
      if (!isValueReceivedOnFaceExpired(f)) {
        // we have a neighbor :)
        byte neighbor = getLastValueReceivedOnFace(f);

        if (myLandType[f] == TREE && neighbor == WILD_FIRE) {
          // then we catch fire
          myLandType[f] = WILD_FIRE;
        }
        else if (neighbor == TREE) {
          numTrees++;
        }
      }

      // check 2 adjacent faces
      if (myLandType[f] == TREE && getLandTypeToMyLeft(f) == WILD_FIRE) {
        // then we catch fire
        myLandType[f] = WILD_FIRE;
      }
      else if (getLandTypeToMyLeft(f) == TREE) {
        numTrees++;
      }

      if (myLandType[f] == TREE && getLandTypeToMyRight(f) == WILD_FIRE) {
        // then we catch fire
        myLandType[f] = WILD_FIRE;
      }
      else if (getLandTypeToMyRight(f) == TREE) {
        numTrees++;
      }


      // turn soil into tree with a higher probability with more trees
      if (random(20) < (numTrees)) {
        if (myLandType[f] == SOIL) {
          setLandTypeOnFace(TREE, f);
        }
      }

    } // done with this face

    // reset the step timer
    stepTimer.set(STEP_DURATION);
  }

  // DISPLAY STATES
  FOREACH_FACE(f) {
    switch (myLandType[f]) {
      case SOIL:      setColorOnFace(OFF, f);    break;
      case TREE:      setColorOnFace(GREEN, f);   break;
      case CTRL_FIRE: setColorOnFace(ORANGE, f);  break;
      case WILD_FIRE: setColorOnFace(RED, f);     break;
    }
  }

  // SHARE MY STATE
  FOREACH_FACE(f) {
    setValueSentOnFace(myLandType[f], f);
  }
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
