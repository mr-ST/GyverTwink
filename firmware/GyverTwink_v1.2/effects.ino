
#define EFF_CANDLE 0
#define EFF_NO_FLAG 1
#define EFF_RISING_WAVES 2
#define EFF_TWINKLE 3
#define EFF_SNOW_FALL 4
#define EFF_WRW 5
#define EFF_RAINBOW 6
#define EFF_CANDY_CANE 7

#define EFF_FIRST EFF_CANDLE
#define EFF_COUNT 8

// Вспомогательная функция для получения позиции по высоте (0-100)
// учитывая что лента идет вверх, потом вниз
int getHeightPos(int i) {
  if (i < STRIP_CENTER) return i; // первая половина идет вверх
  return STRIP_END - i; // вторая половина идет вниз
}

void switchEff() {
  // поиск favorite эффектов
  // while (true) {
  //   if (cfg.rndCh) curEff = random(0, ACTIVE_PALETTES * 2);
  //   else {
  //     if (++curEff >= ACTIVE_PALETTES * 2) curEff = 0;
  //   }
  //   if (effs[curEff].fav) break;
  // }
  curEff++;
  if (curEff >= EFF_COUNT) curEff = EFF_FIRST;
  DEBUG("switch to: ");
  DEBUGLN(curEff);
}

void effects() {
  static Timer effTmr(50);
  static byte prevEff = 255;
  static byte fadeCount = 0;

  if (effTmr.ready()) {
    // эффект сменился
    if (prevEff != curEff) {
      prevEff = curEff;
      fadeCount = 30;
    }

    if (fadeCount) {
      fadeCount--;
      //fadeToBlackBy(leds, LED_MAX, 11);
      if (fadeCount != 0) {
        for (int i = STRIP_START; i <= STRIP_END; ++i) {
          leds[i].fadeToBlackBy(20);
        }
      }
      else {
        for (int i = STRIP_START; i <= STRIP_END; ++i) {
          leds[i] = CRGB::Black;
        }
      }
      strip->showLeds(cfg.bright);
    }
  }
  
  if (fadeCount == 0) {
    switch (curEff)
    {
      case EFF_CANDLE:
        candleTop();
        break;
      case EFF_TWINKLE:
        twinkleChristmas();
        break;
      case EFF_RISING_WAVES:
        risingWaves();
        break;
      case EFF_SNOW_FALL:
        snowfall();
        break;
      case EFF_RAINBOW:
        rainbowSpiral();
        break;
      case EFF_CANDY_CANE:
        candyCane();
        break;
      case EFF_WRW:
        tricolorEffect();
        break;
      case EFF_NO_FLAG:
        norwegianFlagEffect();
        break;
        
      default:
        curEff = EFF_FIRST;
    }
  }
  
  

  /*static Timer effTmr(30);
  static uint16_t countP = 0;
  static byte countSkip = 0;
  static byte prevEff = 255;
  static byte fadeCount = 0;

  if (effTmr.ready()) {
    byte thisEffect;

    if (forceTmr.state()) thisEffect = forceEff;
    else thisEffect = curEff;

    // эффект сменился
    if (prevEff != curEff) {
      prevEff = curEff;
      fadeCount = 25;
    }

    byte scale = effs[thisEffect].scale;
    byte speed = effs[thisEffect].speed;
    byte curPal = thisEffect;
    if (curPal >= ACTIVE_PALETTES) curPal -= ACTIVE_PALETTES;

    for (int i = 0; i < cfg.ledAm; i++) {
      byte idx;

      if (thisEffect < ACTIVE_PALETTES) {
        // первые ACTIVE_PALETTES эффектов - градиент
        // idx = map(xy[i][1], mm.minY, mm.maxY, 0, 255) + counter;   // прямой градиент
        idx = countP + ((mm.w * xy[i][0] / mm.h) + xy[i][1]) * scale / 100;   // диагональный градиент
      } else {
        // следующие - перлин нойс
        idx = inoise8(xy[i][0] * scale / 10, xy[i][1] * scale / 10, countP);
      }
      CRGB color = ColorFromPalette(paletteArr[curPal], idx, 255, LINEARBLEND);

      // плавная смена эффекта
      // меняется за 25 фреймов
      if (fadeCount) leds[i] = blend(leds[i], color, 40);
      else leds[i] = color;
    }
    if (fadeCount) fadeCount--;

    countP += (speed - 128) / 10;
    strip->showLeds(cfg.bright);
  }*/
}

void candleTop() {
  static Timer flame_timer(100);
  static Timer green_timer(400);
  // Основная елка - зеленая
  //fill_solid(leds, LED_MAX, CRGB(0, 100, 0));
  int start = STRIP_CENTER - 7;
  int stop = STRIP_CENTER + 9;

  bool flm = flame_timer.ready();
  bool grn = green_timer.ready();
  if (grn) {
    // Основная елка - зеленая с мерцанием
    for (int i = STRIP_START; i < STRIP_END; i++) {
      // Пропускаем верхушку (она будет пламенем)
      if (i >= start && i < stop) continue;
      
      // Случайное мерцание зеленых диодов
      if (random8() < 10) { // ~12% шанс изменения
        int brightness = random8(10, 255);
        leds[i] = CRGB(0, brightness, 0);
        leds[i].r = 1;
      }
    }
  }

  // Верхние 16 диодов - эффект пламени
  if (flm) {
    for (int i = STRIP_START; i < STRIP_END; i++) {
      // Пропускаем верхушку (она будет пламенем)
      if (i >= start && i < stop) {
        int flicker = random8(50, 255);
        leds[i] = CRGB(flicker, flicker / 2, 0);
        continue;
      }
      // Плавное затухание
      if (leds[i].g > 10) {
        leds[i].fadeToBlackBy(20);
        //leds[i].g = leds[i].g - 1;
      }
    }
  }
  if (flm || grn) {
    strip->showLeds(cfg.bright);
  }
}

// 1. Гирлянда с мерцающими огоньками
void twinkleChristmas() {
  static Timer timer(50);
  static uint8_t hue = 0;
  
  if (timer.ready()) {
    CRGB bkg =  CRGB(0, 10, 0);
    
    // Добавляем случайные огоньки
    if (random8() < 180) {
      int pos = random16(LED_MAX);
      // Чередуем красный, зеленый, золотой, белый
      CRGB colors[] = {CRGB::Red, CRGB::Blue, CRGB(255, 200, 0), CRGB::White};
      leds[pos] = colors[random8(4)];
    }
    for (int i = STRIP_START; i < STRIP_END; ++i) {
      if (leds[i] != bkg) {
        //fadeToBlack(leds, LED_MAX, 10);
        leds[i].fadeToBlackBy(10);
      }
     if (leds[i]  == CRGB::Black) {
      leds[i] = bkg;
     }
    }
    strip->showLeds(cfg.bright);
  }
}

// 3. Волны света снизу вверх
void risingWaves() {
  static uint8_t wave1 = 0;
  static uint8_t wave2 = 50;
  static Timer timer(50);

  if (!timer.ready()) return;

  fill_solid(leds, LED_MAX, CRGB(0, 20, 0)); // темно-зеленый фон
  
  // Первая волна - золотая
  for (int i = 0; i < LED_MAX; i++) {
    int height = getHeightPos(i);
    int brightness = sin8((height * 255 / 100 + wave1) % 255);
    leds[i] += CRGB(brightness, brightness / 2, 0);
  }
  
  // Вторая волна - красная
  for (int i = 0; i < LED_MAX; i++) {
    int height = getHeightPos(i);
    int brightness = sin8((height * 255 / 100 + wave2) % 255);
    leds[i] += CRGB(brightness, 0, 0);
  }
  
  wave1 += 2;
  wave2 += 3;
  
  strip->showLeds(cfg.bright);
}

// 4. Снегопад (белые точки падают вниз)
void snowfall() {
  static Timer timer(80);
  const int flake_count = 2; 
  static uint8_t snowPos[flake_count]; // позиции снежинок
  static bool initialized = false;
  
  if (!initialized) {
    //for (int i = 0; i < flake_count; i++) snowPos[i] = random8(STRIP_CENTER);
    snowPos[0] = STRIP_CENTER - 1;
    snowPos[1] = STRIP_END - STRIP_CENTER / 2; 
    initialized = true;
  }
  
   if (!timer.ready()) return;

  fadeToBlackBy(leds, LED_MAX, 100);
  
  // Елка слегка зеленая
  for (int i = 0; i < LED_MAX; i++) {
    leds[i] += CRGB(0, 20, 0);
  }
  
  // Рисуем снежинки
  for (int i = 0; i < flake_count; i++) {
    // На обеих сторонах
    //if (snowPos[i] < STRIP_CENTER) {
      leds[snowPos[i]] = CRGB::White;
      //leds[STRIP_END - snowPos[i]] = CRGB::White;
    //}
    
    // Падение вниз
    if (snowPos[i] < STRIP_CENTER) {
      snowPos[i]--;
      if (snowPos[i] == 255) snowPos[i] = STRIP_CENTER - 1; // сброс наверх
    }
    else {
      snowPos[i]++;
      if (snowPos[i] >= LED_MAX) snowPos[i] = STRIP_CENTER + 1; // сброс наверх
    }
  }
  
  strip->showLeds(cfg.bright);
}

// 5. Радужная спираль
void rainbowSpiral() {
  static uint8_t hue = 0;
  static Timer timer(30);
  if (!timer.ready()) return;
  for (int i = 0; i < LED_MAX; i++) {
    int height = getHeightPos(i);
    uint8_t pixelHue = hue + (height * 255 / 100);
    leds[i] = CHSV(pixelHue, 255, 200);
  }
  
  hue += 2;
  strip->showLeds(cfg.bright);
}

// 6. Красно-белая полоса (классика)
void candyCane() {
  static uint8_t offset = 0;
  static Timer timer(100);
  if (!timer.ready()) return;

  for (int i = 0; i < LED_MAX; i++) {
    int height = getHeightPos(i);
    if ((height + offset) % 30 < 10) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::White;
    }
  }
  
  offset++;
  strip->showLeds(cfg.bright);
}

// Спецэффект: белый-красный-белый с fade
void tricolorEffect() {
  static unsigned long lastCycleStart = 0;
  static bool effectOn = false;
  static uint8_t brightness = 0;
  
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - lastCycleStart;
  
  // Цикл: 30 сек показ + 5 сек fade out + 5 сек fade in = 40 сек
  if (elapsed >= 40000) {
    lastCycleStart = currentTime;
    elapsed = 0;
  }
  
  // Фазы эффекта
  if (elapsed < 2000) {
    // Fade in (0-2 сек)
    brightness = map(elapsed, 0, 2000, 0, 255);
    effectOn = true;
  } 
  else if (elapsed < 32000) {
    // Полная яркость (2-32 сек = 30 сек показа)
    brightness = 255;
    effectOn = true;
  } 
  else if (elapsed < 35000) {
    // Fade out (32-35 сек)
    brightness = map(elapsed, 32000, 35000, 255, 0);
    effectOn = true;
  } 
  else {
    // Пауза в темноте (35-40 сек)
    brightness = 0;
    effectOn = false;
  }
  
  // Отрисовка эффекта
  if (effectOn || brightness > 0) {
    // Верхняя треть (макушка): белый
    // Левая сторона: индексы 66-99 (верхняя треть от 0 до 99)
    for (int i = 66; i < 100; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
    // Правая сторона: индексы 100-133 (верхняя треть от 100 до 199)
    for (int i = 100; i <= 133; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
    
    // Средняя треть: красный
    // Левая сторона: индексы 33-65
    for (int i = 33; i <= 65; i++) {
      leds[i] = CRGB::Red;
      leds[i].nscale8(brightness);
    }
    // Правая сторона: индексы 134-166
    for (int i = 134; i <= 166; i++) {
      leds[i] = CRGB::Red;
      leds[i].nscale8(brightness);
    }
    
    // Нижняя треть: белый
    // Левая сторона: индексы 0-32
    for (int i = 0; i <= 32; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
    // Правая сторона: индексы 167-199
    for (int i = 167; i < LED_MAX; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
  } else {
    // В паузе гасим всё
    fill_solid(leds, LED_MAX, CRGB::Black);
  }
  
  strip->showLeds(cfg.bright);
  delay(20);
}

// Спецэффект: цвета норвежского флага с fade (3 полосы)
void norwegianFlagEffect() {
  static unsigned long lastCycleStart = 0;
  static bool effectOn = false;
  static uint8_t brightness = 0;
  
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - lastCycleStart;
  
  // Цикл: 30 сек показ + 5 сек fade out + 5 сек fade in = 40 сек
  if (elapsed >= 40000) {
    lastCycleStart = currentTime;
    elapsed = 0;
  }
  
  // Фазы эффекта
  if (elapsed < 2000) {
    // Fade in (0-2 сек)
    brightness = map(elapsed, 0, 2000, 0, 255);
    effectOn = true;
  } 
  else if (elapsed < 32000) {
    // Полная яркость (2-32 сек = 30 сек показа)
    brightness = 255;
    effectOn = true;
  } 
  else if (elapsed < 35000) {
    // Fade out (32-35 сек)
    brightness = map(elapsed, 32000, 35000, 255, 0);
    effectOn = true;
  } 
  else {
    // Пауза в темноте (35-40 сек)
    brightness = 0;
    effectOn = false;
  }
  
  // Отрисовка эффекта
  if (effectOn || brightness > 0) {
    // Макушка на индексе 102 (103-й светодиод)
    // Левая сторона: 0-102, Правая сторона: 103-199
    
    // Полоса 1 (верх/макушка): Красный - 40 диодов
    // Левая сторона: 63-102 (40 диодов), Правая сторона: 103-142 (40 диодов)
    const int r_width = 33;
    const int r_pos_l = STRIP_CENTER - r_width;
    const int r_pos_r = STRIP_CENTER + r_width;
    for (int i = r_pos_l; i <= STRIP_CENTER; i++) {
      leds[i] = CRGB::Red;
      leds[i].nscale8(brightness);
    }
    for (int i = STRIP_CENTER; i <= r_pos_r; i++) {
      leds[i] = CRGB::Red;
      leds[i].nscale8(brightness);
    }
    const int white_pos_l = r_pos_l - 33;
    const int white_pos_r = r_pos_r + 33;
    // Полоса 2 (центр): Белый - 20 диодов (узкая)
    // Левая сторона: 53-62 (10 диодов), Правая сторона: 143-152 (10 диодов)
    for (int i = r_pos_l; i <= white_pos_l; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
    for (int i = r_pos_r; i <= white_pos_r; i++) {
      leds[i] = CRGB::White;
      leds[i].nscale8(brightness);
    }
    
    // Полоса 3 (низ): Синий - 53 диода + 47 диодов
    // Левая сторона: 0-52 (53 диода), Правая сторона: 153-199 (47 диодов)
    const int blue_pos_l = white_pos_l - 33;
    const int blue_pos_r = white_pos_r + 33;
    for (int i = 0; i <= white_pos_l; i++) {
      leds[i] = CRGB::Blue;
      leds[i].nscale8(brightness);
    }
    for (int i = white_pos_r; i < LED_MAX; i++) {
      leds[i] = CRGB::Blue;
      leds[i].nscale8(brightness);
    }
    
  } else {
    // В паузе гасим всё
    fill_solid(leds, LED_MAX, CRGB::Black);
  }
  
  strip->showLeds(cfg.bright);
  delay(20);
}
