#include "common.h"

image *double_size(image *im, real k /*hardness*/) {
  int depth= im->depth;
  if (depth != 1) error("double_size: invalid depth");
  int w= im->width, h= im->height;
  int x, y, dx, dy;
  image *om= make_image(2 * w, 2 * h, im->depth);
  om->ex= 2 * im->ex;
  om->pag= im->pag;
  gray *i1, *i2, *i3, *i4, *o;
  real v;
  real a= 9.0/16, b= 3.0/16, c= 1.0/16; 
  real a1= 8.0/15, b1= 2.0/15, c1= 3.0/15; 
  a= a * (1 - k) + a1 * k;
  b= b * (1 - k) + b1 * k;
  c= c * (1 - k) + c1 * k;

  for (y= 0; y < h; y++) {
    i4= i3= im->channel[0] + (w * y);
    if (y == 0) {i1= i3; i2= i4;}
    else {i1= i3 - w; i2= i4 - w;}
    o= om->channel[0] + (4 * w * y);
    for (x= 0; x < w; x++) {
      v= a * *i4
        + b * (*i3 + *i2)
        + c * *i1 ;
      *o= v; o++;
      if (x > 0) {i1++; i3++;}
      if (x < w - 1) {i2++; i4++;}
      v= a * *i3
        + b * (*i4 + *i1)
        + c * *i2 ;
      *o= v; o++;
    }
    i1= i2= im->channel[0] + (w * y);
    if (y == h - 1) {i3= i1; i4= i2;}
    else {i3= i1 + w; i4= i2 + w;}
    o= om->channel[0] + (4 * w * y) + (2 * w);
    for (x= 0; x < w; x++) {
      v= a * *i2
        + b * (*i1 + *i4)
        + c * *i3 ;
      *o= v; o++;
      if (x > 0) {i1++; i3++;}
      if (x < w - 1) {i2++; i4++;}
      v= a * *i1
        + b * (*i2 + *i3)
        + c * *i4 ;
      *o= v; o++;
    }
  }
  return om;
}

image *half_size(image *im) {
  int depth= im->depth;
  if (depth != 1) error("half_size: invalid depth");
  int w= im->width, h= im->height;
  int x, y;
  int w2= w / 2, h2= h / 2;
  image *om= make_image(w2, h2, im->depth);
  om->ex= im->ex / 2;
  om->pag= im->pag;
  gray *i1, *i2, *i3, *i4, *o;
  real v;

  for (y= 0; y < h; y += 2) {
    i1= im->channel[0] + (w * y); i2= i1 + 1;
    if (y == h - 1) {i3= i1; i4= i2;}
    else {i3= i1 + w; i4= i2 + w;}
    o= om->channel[0] + (w2 * y / 2);
    for (x= 0; x < w; x += 2) {
      v= (*i1 + *i2 + *i3 + *i4) / 4;
      *o= v; o++;
      i1 += 2; i3 += 2;
      if (x < w - 1) {i2 += 2; i4 += 2;}
    }
  }
  return om;
}

image *image_half_x(image *im, int border) {
  int depth= im->depth;
  assert(depth == 1);
  if (border) border= 1;
  int wi= im->width;
  int h= im->height;
  image *om;
  int wo, x, y;
  gray *pi, *po;
  wo= (wi - wi%1) / 2 + border; 
  om= make_image(wo, h, depth);
  if (wi % 2) { // odd
    for (y= 0; y < h; y ++) {
      pi= im->channel[0] + (y * wi);
      po= om->channel[0] + (y * wo);
      if (border) {
        *po= *pi;
        *(po + wo - 1)= *(pi + wi - 1);
        po++; pi++;
      }
      for (x=0; x < wo-2*border; x++,po++,pi+=2) {
        *po= (int)(*pi + *(pi+1)*2 + *(pi+2)) / 4;
      }
    }
  } else { // even
    for (y= 0; y < h; y ++) {
      pi= im->channel[0] + (y * wi);
      po= om->channel[0] + (y * wo);
      if (border) {
        *po= *pi;
        *(po + wo - 1)= *(pi + wi - 1);
        po++; pi++;
      }
      for (x=0; x < wo-2*border; x++,po++,pi+=2) {
        *po= (int)(*pi + *(pi+1)) / 2;
      }
    }
  }
  return om;
}

image *image_half_y(image *im, int border) {
  int depth= im->depth;
  assert(depth == 1);
  if (border) border= 1;
  int w= im->width;
  int hi= im->height;
  image *om;
  int ho, x, y;
  gray *pi, *po;
  ho= (hi - hi%2) / 2 + border; 
  om= make_image(w, ho, depth);
  if (border) {
    po= om->channel[0];
    pi= im->channel[0];
    memcpy(po, pi, w * sizeof(*po));
    po += (ho - 1) * w;
    pi += (hi - 1) * w;
    memcpy(po, pi, w * sizeof(*po));
  }
  if (hi % 2) { // odd
    for (y= border; y < ho - border; y ++) {
      po= om->channel[0] + w * y;
      pi= im->channel[0] + w * (y*2 -border +1);
      for (x=0; x < w; x++, po++, pi++) {
        *po= (int)(*(pi-w) + (*pi)*2 + *(pi+w)) / 4;
      }
    }
  } else { // even
    for (y= border; y < ho - border; y ++) {
      po= om->channel[0] + w * y;
      pi= im->channel[0] + w * (y*2 -border +1);
      for (x=0; x < w; x++, po++, pi++) {
        *po= (int)(*(pi-w) + (*pi)) / 2;
      }
    }
  }
  return om;
}

image *image_double_x(image *im, int border, int odd) {
  int depth= im->depth;
  assert(depth == 1);
  if (border) border= 1;
  if (odd) odd= 1;
  int wi= im->width;
  int h= im->height;
  image *om;
  int wo, x, y;
  gray *pi, *po;
  wo= (wi - border)*2 + odd; 
  om= make_image(wo, h, depth);
  pi= im->channel[0];
  po= om->channel[0];
  if (odd) {
    // border:
      // i: 0   1   2   3   4
      // o: 0 1 2 3 4 5 6 7 8
    // no border:
      // i:   0   1   2   3
      // o: 0 1 2 3 4 5 6 7 8
    for (y= 0; y < h; y ++) {
      *po= *pi; po++;
      // i=0, o=1
      if (border) {
        *po= (int)(*pi + *(pi+1)) /2 ;
        po++; pi++;
        // i=1, o=2
      }
      for (x=0; x<wi-1-border; x++) {
        *po= *pi; po++;
        *po= (int)(*pi + *(pi+1)) / 2;
        po++; pi++;
      }
      if (! border) { *po= *pi; po++; }
      *po= *pi;
      po++; pi++;
    }
  } else { // even
    // border:
      // i: 0  1   2   3  4
      // o: 0 1 2 3 4 5 6 7
    // no border:
      // i:  0   1   2   3
      // o: 0 1 2 3 4 5 6 7
    for (y= 0; y < h; y++) {
      *po= *pi; po++;
      // i=0, o=1
      if (border) {
        *po= (int)(*pi + *(pi+1) * 3) / 4;
        po++; pi++;
        // i=1, o=2
      }
      for (x=0; x < wi-1-2*border; x++) {
        *po= (int)(*pi * 3 + *(pi+1)) / 4;
        po++;
        *po= (int)(*pi + *(pi+1) * 3) / 4;
        po++; pi++;
      }
      if (border) {
        *po= (int)(*pi * 3 + *(pi+1)) / 4;
        po++; pi++;
      }
      *po= *pi;
      po++; pi++;
    }
  }
  return om;
}

image *image_double_y(image *im, int border, int odd) {
  int depth= im->depth;
  assert(depth == 1);
  if (border) border= 1;
  if (odd) odd= 1;
  int w= im->width;
  int hi= im->height;
  image *om;
  int ho, x, i;
  gray *pi, *po;
  ho= (hi - border)*2 +odd;
  om= make_image(w, ho, depth);
  po= om->channel[0];
  pi= im->channel[0];
  // i= o= 0
  memcpy(po, pi, w * sizeof(*po));
  po += w;
  // i=0, o=1
  if (odd) {
    // border:
      // i: 0   1   2   3   4
      // o: 0 1 2 3 4 5 6 7 8
    // no border:
      // i:   0   1   2   3
      // o: 0 1 2 3 4 5 6 7 8
    if (border) {
      for (x=0; x<w; x++,po++,pi++) {
        *po= (int)(*pi + *(pi+w)) / 2;
        // i=1, o=2
      }
    }
    for (i=0; i<hi-1-border; i++) {
      memcpy(po, pi, w * sizeof(*po));
      po += w;
      for (x=0; x<w; x++,po++,pi++) {
        *po= (int)(*pi + *(pi+w)) / 2;
      }
    }
    if (! border) {
      memcpy(po, pi, w * sizeof(*po));
      po += w;
    }
  } else { // even
    // border:
      // i: 0  1   2   3  4
      // o: 0 1 2 3 4 5 6 7
    // no border:
      // i:  0   1   2   3
      // o: 0 1 2 3 4 5 6 7
    memcpy(po, pi, w * sizeof(*po));
    po += w;
    // i=0, o=1
    if (border) {
      for (x=0; x<w; x++,po++,pi++) {
        *po= (int)(*pi + *(pi+w) * 3) / 4;
      }
      // i=1, o=2
    }
    for (i=0; i<hi-1-2*border; i++,po+=w) {
      for (x=0; x<w; x++,po++,pi++) {
        *po= (int)(*pi * 3 + *(pi+w)) / 4;
        *(po+w)= (int)(*pi + *(pi+w)*3) / 4;
      }
    }
    if (border) {
      // i=hi-2, o=ho-2
      for (x=0; x<w; x++,po++,pi++) {
        *po= (int)(*pi * 3 + *(pi+w)) / 4;
      }
    }
  }
  // i=hi-1, o=ho-1
  memcpy(po, pi, w * sizeof(*po));
  return om;
}


// vim: sw=2 ts=2 sts=2 expandtab:
