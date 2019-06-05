/*  Forest Fire
 *  by Jonathan Bobrow, Move38
 *  06/04/2019
 *  
 *  Press a Tree to control burn (no spreading)
 *  Long Press a Tree to strike lightning, it will ignite and then spread
 *  
 *  Plots of land grow trees (quicker around other Trees)... Each plot has
 *  up to 6 Trees
 *  The opportunity for a Tree to grow happens 3 times per second and fire
 *  spreads at this rate as well.
 *  The forest shares a heartbeat, so trees grow at the same time
 */

// firefly style sync

enum LandType {
  SOIL,
  TREE,
  CTRL_FIRE,
  WILD_FIRE
};

byte myLandType = SOIL;

#define LIGHT_DURATION 600
Timer lighteningStrikeTimer;
bool didLighteningStrike = false;


#define STEP_DURATION 1000
Timer stepTimer;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  if(buttonSingleClicked()) {
    myLandType = CTRL_FIRE;
  }

  if(buttonLongPressed()) {
    didLighteningStrike = true;
    lighteningStrikeTimer.set(LIGHT_DURATION);
  }

  if(didLighteningStrike && lighteningStrikeTimer.isExpired()) {
    myLandType = WILD_FIRE;
    didLighteningStrike = false;
  }

  if(stepTimer.isExpired()) {
  // ready to evaluate world

    // burn out after we have been fire
    if(myLandType == WILD_FIRE || myLandType == CTRL_FIRE) {
      myLandType = SOIL;  
    }
    
    byte numTrees = 0;
    
    FOREACH_FACE(f) {
      if(!isValueReceivedOnFaceExpired(f)) {
        // we have a neighbor :)
        byte neighbor = getLastValueReceivedOnFace(f);
  
        if(neighbor == WILD_FIRE) {
          // then we catch fire
          myLandType = WILD_FIRE;
        }
        else if(neighbor == TREE) {
          numTrees++;
        }
      }
    }

    // turn soil into tree with a higher probability with more trees
    if(random(10) < (numTrees + 4)) {
      if(myLandType == SOIL) {
        myLandType = TREE;
      }
    }
    
    stepTimer.set(STEP_DURATION);
  }
  
  // DISPLAY STATES
  switch(myLandType) {
    case SOIL: setColor(BLUE);        break;
    case TREE: setColor(GREEN);       break;
    case CTRL_FIRE: setColor(ORANGE); break;
    case WILD_FIRE: setColor(RED);    break;
  }

  // SHARE MY STATE
  setValueSentOnAllFaces(myLandType);
}
