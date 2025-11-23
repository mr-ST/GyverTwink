
#define EFF_CANDLE 0
#define EFF_FALLING_STARS 1
#define EFF_RISING_WAVES 2
#define EFF_TWINKLE 3
#define EFF_SNOW_FALL 4
#define EFF_WRW 5
#define EFF_RAINBOW 6
#define EFF_CANDY_CANE 7
#define EFF_NO_FLAG 8
#define EFF_CHASING_LIGHTS 9
#define EFF_SNAKE 10
#define EFF_SNAKE1 11
#define EFF_HEART 12
#define EFF_AURORA 13
#define EFF_FADE_IN_OUT 14

#define EFF_FIRST EFF_CANDLE
#define EFF_COUNT 15

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
      case EFF_CHASING_LIGHTS:
        chasingLights();
        break;
      case EFF_SNAKE:
        snake();
        break;
      case EFF_SNAKE1:
        snake1();
        break;
      case EFF_HEART:
        breathingHeart();
        break;
      case EFF_AURORA:
        aurora();
        break;
      case EFF_FADE_IN_OUT:
        evenOddPulse();
        break;
      case EFF_FALLING_STARS:
        fallingStars();
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

// 8. Бегущие огни по спирали
void chasingLights() {
  static uint32_t position = 0;
  static Timer timer(50);
  if (!timer.ready()) return;

  fadeToBlackBy(leds, LED_MAX, 50);
  
  // Три цветных точки бегут по спирали
  CRGB colors[] = {CRGB::Red, CRGB::Gold, CRGB::Green};
  
  for (uint8_t i = 0; i < 3; i++) {
    uint32_t pos = (position + i * 66) % LED_MAX;
    leds[pos] = colors[i];
    // Хвост
    if (pos > 0) leds[pos - 1] = colors[i];
    if (pos > 1) leds[pos - 2] = colors[i];
  }
  
  position++;
  strip->showLeds(cfg.bright);
}

// 9. Змейка - огоньки бегут от начала до конца ленты
void snake() {
  static uint32_t position = 0; // используем uint16_t для больших значений
  static const uint8_t SNAKE_COUNT = 2; // количество змеек
  static const uint8_t SNAKE_LENGTH = 30; // длина хвоста каждой змейки
  static Timer timer(20);
  if (!timer.ready()) return;

  fadeToBlackBy(leds, LED_MAX, 80);
  
  // Цвета для змеек
  CRGB colors[] = {CRGB::Red, /*CRGB::Blue,*/ CRGB::Yellow4};
  
  for (uint8_t s = 0; s < SNAKE_COUNT; s++) {
    // Позиция головы змейки с отступом между змейками
    uint32_t headPos = (position + s * (LED_MAX / SNAKE_COUNT)) % LED_MAX;
    
    // Рисуем змейку с затухающим хвостом
    for (uint8_t j = 0; j < SNAKE_LENGTH; j++) {
      // Правильное вычисление позиции хвоста с учетом переполнения
      int pos;
      if (headPos >= j) {
        pos = headPos - j;
      } else {
        pos = LED_MAX - (j - headPos);
      }
      
      uint8_t brightness = 255 - (j * 255 / SNAKE_LENGTH);
      leds[pos] = colors[s];
      leds[pos].fadeToBlackBy(255 - brightness);
    }
  }
  
  position++;
  if (position >= LED_MAX * 100) position = 0; // сброс для избежания переполнения
  
  strip->showLeds(cfg.bright);
}

void snake1() {
  static uint32_t position = 0; // используем uint16_t для больших значений
  static const uint8_t SNAKE_COUNT = 1; // количество змеек
  static const uint8_t SNAKE_LENGTH = 100; // длина хвоста каждой змейки
  static Timer timer(40);
  if (!timer.ready()) return;

  fadeToBlackBy(leds, LED_MAX, 80);
  
  // Цвета для змеек
  CRGB colors[] = {CRGB::LawnGreen, /*CRGB::Blue,*/ CRGB::Yellow4};
  
  for (uint8_t s = 0; s < SNAKE_COUNT; s++) {
    // Позиция головы змейки с отступом между змейками
    uint32_t headPos = (position + s * (LED_MAX / SNAKE_COUNT)) % LED_MAX;
    
    // Рисуем змейку с затухающим хвостом
    for (uint8_t j = 0; j < SNAKE_LENGTH; j++) {
      // Правильное вычисление позиции хвоста с учетом переполнения
      int pos;
      if (headPos >= j) {
        pos = headPos - j;
      } else {
        pos = LED_MAX - (j - headPos);
      }
      
      uint8_t brightness = 255 - (j * 255 / SNAKE_LENGTH);
      leds[pos] = colors[s];
      leds[pos].fadeToBlackBy(255 - brightness);
    }
  }
  
  position++;
  if (position >= LED_MAX * 100) position = 0; // сброс для избежания переполнения
  
  strip->showLeds(cfg.bright);
}

// 10. Пульсирующее сердце - ёлка "дышит" светом
void breathingHeart() {
  static uint8_t phase = 0;
  static uint8_t colorMode = 0; // 0 = красный, 1 = зелёный
  static uint32_t lastColorChange = 0;
  static Timer timer(30);
  if (!timer.ready()) return;

  // Меняем цвет каждые 10 секунд
  if (millis() - lastColorChange > 20000) {
    colorMode = 1 - colorMode;
    lastColorChange = millis();
  }
  
  // Базовая яркость с использованием синуса для плавной пульсации
  uint8_t baseBrightness = sin8(phase);
  
  for (int i = 0; i < LED_MAX; i++) {
    int height = getHeightPos(i);
    
    // Градиент от основания к вершине (вершина ярче)
    uint8_t heightFactor = map(height, 0, 79, 80, 255);
    uint8_t brightness = scale8(baseBrightness, heightFactor);
    
    // Минимальная яркость для видимости
    brightness = map(brightness, 0, 255, 1, 255);
    
    if (colorMode == 0) {
      // Красное пульсирующее сердце
      leds[i] = CRGB(brightness, 0, 0);
    } else {
      // Зелёное пульсирующее сердце
      leds[i] = CRGB(0, brightness, 0);
    }
  }
  
  phase += 2; // скорость пульсации
  
  strip->showLeds(cfg.bright);
}

///////////////////////////// aurora
float t = 0;
float speed = 0.025;
float scale = 0.018;
#define SPIRAL_LEN  100

inline int LED1(int i) { return i; }
inline int LED2(int i) { return i + SPIRAL_LEN; }

// Генерация северного сияния
CRGB aurora(float pos, float t, float phaseShift) {

  // Цвет — область “сияния”: зелёный → бирюза → синий → фиолетовый
  //uint8_t hue = 100 + sin(pos * 2.7 + t * 0.9 + phaseShift) * 50;
  uint8_t hue = 130 + sin(pos * 3 + t * 0.7 + phaseShift) * 15;

  // Насыщенность почти максимальная (важно для исключения белого)
  uint8_t sat = 240;

  // Яркость колеблется шумом — именно это даёт "живость"
  uint8_t val = inoise8(pos * 90 + phaseShift * 1200, t * 140);

  return CHSV(hue, sat, val);
}

void aurora() {
  static Timer timer(10);
  if (!timer.ready()) return;

  t += speed;

  for (int i = 0; i < SPIRAL_LEN; i++) {

    float pos = i * scale;

    // “Дыхание” волны
    float wave = sin(pos * 5.0 + t * 1.7) * 0.5 + 0.5;
    uint8_t fade = (1.0 - wave) * 110;

    // ------- Спираль 1 -------
    CRGB c1 = aurora(pos, t, 0.0);
    c1.fadeToBlackBy(fade);
    leds[LED1(i)] = c1;

    // ------- Спираль 2 (цветовой сдвиг) -------
    CRGB c2 = aurora(pos, t, 2.4);
    c2.fadeToBlackBy(fade);
    leds[LED2(i)] = c2;
  }

  strip->showLeds(cfg.bright);
}

// 11. Чётные и нечётные - противофазная пульсация
void evenOddPulse() {
  static uint8_t phase = 0;
  
  static Timer timer(40);
  if (!timer.ready()) return;

  // Яркость для чётных диодов (прямая фаза)
  uint8_t evenBrightness = sin8(phase);
  // Яркость для нечётных диодов (противоположная фаза)
  uint8_t oddBrightness = sin8(phase + 128); // сдвиг на 180 градусов
  
  // Минимальная яркость для видимости
  evenBrightness = map(evenBrightness, 0, 255, 1, 255);
  oddBrightness = map(oddBrightness, 0, 255, 1, 255);
  
  for (int i = 0; i < LED_MAX; i++) {
    if (i % 2 == 0) {
      // Чётные диоды - фиолетовый цвет
      leds[i] = CRGB(evenBrightness, 0, evenBrightness);
    } else {
      // Нечётные диоды - белый цвет
      leds[i] = CRGB(oddBrightness, oddBrightness, oddBrightness);
    }
  }
  
  phase += 1; // скорость пульсации
  
  strip->showLeds(cfg.bright);
}

// 12. Падающие звёзды - звёзды падают и накапливаются снизу
void fallingStars() {
  static uint8_t filledHeight = 0; // до какой высоты заполнено (в звёздах)
  static uint8_t fallingStarPos = 99; // текущая позиция падающей звезды
  static CRGB fallingStarColor = CRGB::Red; // цвет падающей звезды
  static bool starFalling = false; // падает ли звезда сейчас
  static uint32_t lastUpdate = 0;
  static CRGB stackColors[34]; // цвета накопленных звёзд (100/3 = ~33 звезды)
  static bool initialized = false;
  
  static Timer timer(10);
  if (!timer.ready()) return;

  // Инициализация
  if (!initialized) {
    for (int i = 0; i < 34; i++) {
      stackColors[i] = CRGB::Black;
    }
    initialized = true;
  }
  
  // Проверка на полное заполнение - сброс
  if (filledHeight >= 33) {
    filledHeight = 0;
    for (int i = 0; i < 34; i++) {
      stackColors[i] = CRGB::Black;
    }
    fill_solid(leds, LED_MAX, CRGB::Black);
    //delay(500); // пауза перед новым циклом
  }
  
  // Запуск новой звезды если нет падающей
  if (!starFalling) {
    starFalling = true;
    fallingStarPos = 99;
    // Случайный цвет
    uint8_t colorChoice = random8(6);
    switch(colorChoice) {
      case 0: fallingStarColor = CRGB::Red; break;
      case 1: fallingStarColor = CRGB::Gold; break;
      case 2: fallingStarColor = CRGB::Blue; break;
      case 3: fallingStarColor = CRGB::White; break;
      case 4: fallingStarColor = CRGB::Green; break;
      case 5: fallingStarColor = CRGB(255, 0, 255); break; // фиолетовый
    }
  }
  
  // Обновление позиции каждые 60мс
  if (millis() - lastUpdate > 10) {
    lastUpdate = millis();
    
    if (starFalling) {
      // Движение звезды вниз
      uint8_t stopPosition = filledHeight * 3; // каждая звезда занимает 3 диода
      if (fallingStarPos > stopPosition + 2) {
        fallingStarPos--;
      } else {
        // Звезда остановилась - добавляем в стек
        stackColors[filledHeight] = fallingStarColor;
        filledHeight++;
        starFalling = false;
      }
    }
  }
  
  // Отрисовка
  fill_solid(leds, LED_MAX, CRGB::Black);
  
  // Рисуем накопленные звёзды на обеих спиралях (каждая звезда = 3 диода)
  for (uint8_t s = 0; s < filledHeight; s++) {
    uint8_t basePos = s * 3;
    // Первая спираль (0-99)
    for (uint8_t i = 0; i < 3; i++) {
      if (basePos + i < 100) {
        leds[basePos + i] = stackColors[s];
      }
    }
    // Вторая спираль (100-199), зеркально
    for (uint8_t i = 0; i < 3; i++) {
      if (basePos + i < 100) {
        leds[199 - (basePos + i)] = stackColors[s];
      }
    }
  }
  
  // Рисуем падающую звезду (3 точки)
  if (starFalling) {
    // Первая спираль
    for (uint8_t i = 0; i < 3; i++) {
      if (fallingStarPos >= i && fallingStarPos - i < 100) {
        leds[fallingStarPos - i] = fallingStarColor;
      }
    }
    
    // Вторая спираль
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t secondPos = 199 - (fallingStarPos - i);
      if (secondPos < LED_MAX && fallingStarPos >= i) {
        leds[secondPos] = fallingStarColor;
      }
    }
  }
  
  strip->showLeds(cfg.bright);
}

