
#include "Mock_HAL_XPT2046_Touchscreen.h"
#include "Mock_HAL_TFT.h"
#include "Mock_Compat.h"

#include "ExtendedTouchEvent.h"
#include <gtest/gtest.h>
#include <list>
#include <cstdint>
#include <ostream>

#ifdef ESP8266
// d1_mini needs this include, otherwise test compile fails due to missing includes
    #include <SPI.h>
#else
    // parametrized tests seem to be to complex for an ESP8266 (crash due to OOM)
    #include "WipeTestSuite.h"
    #include "UnrawTestSuite.h"
#endif

using ::testing::Eq;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::Sequence;

//#####################################################################

// saves data from fired events during test
struct ts_event {
public:
    ts_event(int16_t x, int16_t y, int16_t z, EVENT e) : _altp(HAL_TouchscreenPoint(x, y, z)), _event(e) {};
    ts_event(HAL_TouchscreenPoint p, EVENT e) : _altp(p), _event(e) {};

    HAL_TouchscreenPoint _altp;
    EVENT _event;
};

// handles events during tests and save them for later assertion
class event_handler_for_tests {
public:
    event_handler_for_tests() {
        referenceIt = this;
    }

    ~ event_handler_for_tests() {
        referenceIt = nullptr;
    }

    static event_handler_for_tests *referenceIt;

    static void callback_allevents(HAL_TouchscreenPoint & p, EVENT event) {
        referenceIt -> all_events.push_back(ts_event(p, event));
    }

    static void ts_Point_callback(HAL_TouchscreenPoint & p) {
        referenceIt -> single_events.push_back(p);
    }

    static void wipe_callback(WIPE_DIRECTION direction) {
        referenceIt -> wipe_events.push_back(direction);
    }

    static void checkEvents(std::list<WIPE_DIRECTION> expected_events) {
        auto events = referenceIt -> wipe_events;
        ASSERT_EQ(events.size(), expected_events.size());

        int i = 0;
        for (WIPE_DIRECTION wd : events) {
            auto excpected = expected_events.front();
            ASSERT_EQ(wd, excpected) << " i=" << i;
            expected_events.pop_front();
            i++;
        }
    }

    static void checkEvents(std::list<ts_event> expected_events) {
        auto events = referenceIt -> all_events;
        ASSERT_EQ(events.size(), expected_events.size());

        int i = 0;
        for (ts_event tsevent : events) {
            ASSERT_TRUE(& tsevent._altp) << " i=" << i;
            ASSERT_TRUE(& tsevent._event) << " i=" << i;

            auto excpected = expected_events.front();
            ASSERT_EQ(tsevent._altp, excpected._altp) << "ts_event.HAL_TouchscreenPoint i=" << i;
            ASSERT_EQ(tsevent._event, excpected._event)<< "ts_event.EVENT i=" << i;

            expected_events.pop_front();
            i++;
        }
    }

    static void checkEvents(std::list<HAL_TouchscreenPoint> expected_events) {
        auto events = referenceIt -> single_events;
        ASSERT_EQ(events.size(), expected_events.size());

        int i = 0;
        for (HAL_TouchscreenPoint altp_event : events) {
            auto excpected = expected_events.front();
            ASSERT_EQ(altp_event, excpected) << "HAL_TouchscreenPoint i=" << i;

            expected_events.pop_front();
            i++;
        }
    }

private:
    std::list<ts_event> all_events;
    std::list<HAL_TouchscreenPoint> single_events;
    std::list<WIPE_DIRECTION> wipe_events;
};

// C++ is so crazy .....
event_handler_for_tests * event_handler_for_tests::referenceIt;

// for converting to raw coordinates from tft
struct raw {
    raw(int16_t x, int16_t y) : x(x), y(y) {}
    ~raw() = default;

    int16_t x, y;
};

static raw toRaw(ROTATION rotation, int16_t x, int16_t y) {
    int16_t xraw, yraw;

    // see the wierd calculation in update() from XPT2046_Touchscreen.cpp
    switch (rotation) {
        case ROTATION::PORTRAIT_TR:
            xraw = 4095 - y;
            yraw = x;
            break;
        case ROTATION::LANDSCAPE_BR:
            xraw = x;
            yraw = y;
            break;
        case ROTATION::PORTRAIT_BL:
            xraw = x;
            yraw = 4095 - y;
            break;
        case ROTATION::LANDSCAPE_TL:
            xraw = 4095 - x;
            yraw = 4095 - y;
            break;
    }

    printf("******* toRaw x=%d, y=%d -> xraw=%d, yraw=%d **********\n", x, y, xraw, yraw);

    return raw(xraw, yraw);
}

//#####################################################################

static void checkEvent(std::list<ts_event> & events, int16_t x, int16_t y, int16_t z, EVENT e) {
    auto tsevent = events.front();
    ASSERT_TRUE(& tsevent._altp);
    ASSERT_TRUE(& tsevent._event);

    ASSERT_EQ(HAL_TouchscreenPoint(x, y, z), tsevent._altp);
    ASSERT_EQ(tsevent._event, e);

    events.pop_front();
}
//#####################################################################

// copy from WMath.cpp
static long WMathmap(long n, long in_min, long in_max, long out_min, long out_max) {
    const long run = in_max - in_min;
    if(run == 0){
        return -1; // AVR returns -1, SAM returns 0
    }
    const long rise = out_max - out_min;
    const long delta = n - in_min;
    auto mapped = (delta * rise) / run + out_min;
    printf("******* WMathmap n=%ld, in_min=%ld, in_max=%ld, out_min=%ld, out_max=%ld -> mapped=%ld **********\n",
           n, in_min, in_max, out_min, out_max, mapped);
    return mapped;
}
//#####################################################################

static void mock_ll_map(const Sequence & seq,
                        Mock_Compat & compat,
                        long x, long in_min, long in_max, long out_min, long out_max) {
    EXPECT_CALL(compat,  ll_map(Matcher<long>(Eq(x)),
                                Matcher<long>(Eq(in_min)),
                                Matcher<long>(Eq(in_max)),
                                Matcher<long>(Eq(out_min)),
                                Matcher<long>(Eq(out_max))))
            .InSequence(seq)
            .WillOnce(Return(WMathmap(x, in_min, in_max, out_min, out_max)))
            .RetiresOnSaturation();
}
//#####################################################################

static void mockToScreen(const Sequence & seq,
                        Mock_TFT & tft,
                        Mock_Compat & compat,
                        Mock_XPT2046_Touchscreen & ts,
                        int16_t x, int16_t y, int16_t z,
                        long millis,
                        ROTATION rotation = ROTATION::PORTRAIT_TR,
                        bool coordinatesAreRaw = true) {
    // called by toScreen()
    EXPECT_CALL(compat, ll_millis()).InSequence(seq)
                                    .WillOnce(Return(millis))
                                    .RetiresOnSaturation();
    int w, h;
    // values of getHeight()/getWidth() change with rotation
    switch (rotation) {
        case ROTATION::PORTRAIT_TR :
        case ROTATION::PORTRAIT_BL :
            h = SCREENY;
            w = SCREENX;
            break;
        case ROTATION::LANDSCAPE_BR :
        case ROTATION::LANDSCAPE_TL :
            h = SCREENX;
            w = SCREENY;
            break;
    }

    EXPECT_CALL(tft, width()).InSequence(seq)
                             .WillOnce(Return(w))
                             .RetiresOnSaturation();
    EXPECT_CALL(tft, height()).InSequence(seq)
                              .WillOnce(Return(h))
                              .RetiresOnSaturation();

    EXPECT_CALL(ts, coordinatesAreRaw()).InSequence(seq)
            .WillOnce(Return(coordinatesAreRaw))
            .RetiresOnSaturation();

    // calls to getRotation() and ll_map() only when raw coordinates are returned from device
    if (coordinatesAreRaw) {
        EXPECT_CALL(tft, getRotation()).InSequence(seq)
                                       .WillOnce(Return(rotation))
                                       .RetiresOnSaturation();


        // values for 1st call ll_map
        long in_min, in_max, out_min, out_max;
        // values for 2nd call ll_map
        long in_min2, in_max2, out_min2, out_max2;
        switch (rotation) {
            case ROTATION::PORTRAIT_TR :
                in_min = TFT_SMALL_MIN;
                in_max = TFT_SMALL_MAX;
                out_min = 0;
                out_max = w;
                in_min2 = TFT_LONG_MIN;
                in_max2 = TFT_LONG_MAX - BLACKEDGE;
                out_min2 = 0;
                out_max2 = h;
                break;
            case  ROTATION::LANDSCAPE_BR :
                in_min = TFT_LONG_MIN;
                in_max = TFT_LONG_MAX - BLACKEDGE;
                out_min = 0;
                out_max = w;
                in_min2 = TFT_SMALL_MIN;
                in_max2 = TFT_SMALL_MAX;
                out_min2 = 0;
                out_max2 = h;
                break;
            case  ROTATION::PORTRAIT_BL :
                in_min = TFT_SMALL_MIN;
                in_max = TFT_SMALL_MAX;
                out_min = 0;
                out_max = w;
                in_min2 = TFT_LONG_MIN + BLACKEDGE;
                in_max2 = TFT_LONG_MAX;
                out_min2 = 0;
                out_max2 = h;
                break;
            case  ROTATION::LANDSCAPE_TL :
                in_min = TFT_LONG_MIN + BLACKEDGE;
                in_max = TFT_LONG_MAX;
                out_min = 0;
                out_max = w;
                in_min2 = TFT_SMALL_MIN;
                in_max2 = TFT_SMALL_MAX;
                out_min2 = 0;
                out_max2 = h;
                break;
    }

    printf("******* mockToScreen x=%d, in_min=%ld, in_max=%ld, out_min=%ld, out_max=%ld, rotation=%d **********\n",
           x, in_min, in_max, out_min, out_max, (int) rotation);
    mock_ll_map(seq, compat, x, in_min, in_max, out_min, out_max);

    printf("******* mockToScreen y=%d, in_min2=%ld, in_max2=%ld, out_min2=%ld, out_max2=%ld, rotation=%d **********\n",
           y, in_min2, in_max2, out_min2, out_max2, (int) rotation);
    mock_ll_map(seq, compat, y, in_min2, in_max2, out_min2, out_max2);

    printf("\n");
    }
}
//#####################################################################

static void preparePoll(const Sequence & seq,
                        Mock_XPT2046_Touchscreen & ts,
                        Mock_TFT & tft,
                        Mock_Compat & compat,
                        int16_t x, int16_t y, int16_t z,
                        long millis,
                        ROTATION rotation = ROTATION::PORTRAIT_TR,
                        bool coordinatesAreRaw = true) {

    printf("******* preparePoll x=%d, y=%d, rotation=%d **********\n", x, y, (int) rotation);

    auto rawxy = coordinatesAreRaw ? toRaw(rotation, x, y) : raw(x, y);

    EXPECT_CALL(ts, getPoint()).InSequence(seq)
                               .WillOnce(Return(HAL_TouchscreenPoint(rawxy.x, rawxy.y, z)))
                               .RetiresOnSaturation();
    // one call of ll_millis()
    EXPECT_CALL(compat, ll_millis()).InSequence(seq)
                                    .WillOnce(Return(millis))
                                    .RetiresOnSaturation();
    // one call of toScreen()
    mockToScreen(seq, tft, compat, ts, rawxy.x, rawxy.y, z, millis, rotation, coordinatesAreRaw);
}
//#####################################################################

// ESP8266 crashes with OOM - may be too complex?
#ifndef ESP8266
// this test runs parametrized
TEST_P(WipeTestSuite, test_setRotation) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);
    // Parameter comes from gtest
    auto rot = GetParam().rot;

    EXPECT_CALL(tft, setRotation(Matcher<ROTATION>(Eq(rot)))).Times(1);
    EXPECT_CALL(ts,  setRotation(Matcher<ROTATION>(Eq(rot)))).Times(1);

    ete.setRotation(rot);
}
#endif
//#####################################################################

//// ESP8266 crashes with OOM - may be too complex?
#ifndef ESP8266
// this test runs parametrized
TEST_P(UnrawTestSuite, test_Touch) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDown(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined

    auto rot = GetParam().rot;
    const Sequence seq;
    auto x = GetParam().x;
    auto y = GetParam().y;

    preparePoll(seq, ts, tft, compat, x, y, 25, 10L, rot, false);

    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) {ts_event(x, y, 0, EVENT::EVT_DOWN)});
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(x, y, 0) } );
}
#endif
//#####################################################################

TEST(test_ExtendedTouchEvent, test_isInRectangle) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);

    HAL_TouchscreenPoint p = HAL_TouchscreenPoint(20, 30, 8);
    HAL_TouchscreenPoint p2 = HAL_TouchscreenPoint(20, 30, 8);

    EXPECT_TRUE(ete.isInRectangle(p, 2, 5, 20, 30));
    EXPECT_TRUE(ete.isInRectangle(p, 2, 5, 18, 25));
    EXPECT_FALSE(ete.isInRectangle(p, 2, 5, 17, 25));
    EXPECT_FALSE(ete.isInRectangle(p, 2, 5, 18, 24));
    EXPECT_FALSE(ete.isInRectangle(p, 2, 5, 18, 24));
    EXPECT_FALSE(ete.isInRectangle(p, 21, 5, 20, 30));
    EXPECT_FALSE(ete.isInRectangle(p, 2, 35, 20, 30));
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_isInCircle) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);
    auto p1 = HAL_TouchscreenPoint(20, 28, 8);
    EXPECT_TRUE(ete.isInCircle(p1, 2, 5, 30));
    auto p2 = HAL_TouchscreenPoint(20, 30, 8);
    EXPECT_FALSE(ete.isInCircle(p2, 2, 5, 30));
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_isInPieSlice) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    int16_t pieangles[] = { 60, 30, 120, 150 };

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);

    auto p1 = HAL_TouchscreenPoint(150, 190, 0);
    EXPECT_EQ(0, ete.isInPieSlice(p1, 100, 200, 60, 4, pieangles));

    auto p2 = HAL_TouchscreenPoint(110, 150, 0);
    EXPECT_EQ(1, ete.isInPieSlice(p2, 100, 200, 60, 4, pieangles));

    auto p3 = HAL_TouchscreenPoint(150, 210, 0);
    EXPECT_EQ(3, ete.isInPieSlice(p3, 100, 200, 60, 4, pieangles));

    auto p4 = HAL_TouchscreenPoint(30, 210, 0);
    EXPECT_EQ(-1, ete.isInPieSlice(p4, 100, 200, 60, 4, pieangles));

    // simulates an incomplete piechart
    auto p5 = HAL_TouchscreenPoint(30, 210, 0);
    EXPECT_EQ(-1, ete.isInPieSlice(p5, 100, 200, 60, 3, pieangles));
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_isInPieSlicePercents) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    float piepercent[] = { 1.f/6, 1.f/12, 1.f/3, 150.f/360 };

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);

    auto p1 = HAL_TouchscreenPoint(150, 190, 0);
    EXPECT_EQ(0, ete.isInPieSlice(p1, 100, 200, 60, 4, piepercent));

    auto p2 = HAL_TouchscreenPoint(110, 150, 0);
    EXPECT_EQ(1, ete.isInPieSlice(p2, 100, 200, 60, 4, piepercent));

    auto p3 = HAL_TouchscreenPoint(150, 210, 0);
    EXPECT_EQ(3, ete.isInPieSlice(p3, 100, 200, 60, 4, piepercent));

    auto p4 = HAL_TouchscreenPoint(30, 210, 0);
    EXPECT_EQ(-1, ete.isInPieSlice(p4, 100, 200, 60, 4, piepercent));

    // simulates an incomplete piechart
    auto p5 = HAL_TouchscreenPoint(30, 210, 0);
    EXPECT_EQ(-1, ete.isInPieSlice(p5, 100, 200, 60, 3, piepercent));
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_isInPieSliceChecks) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;


    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);

    auto p = HAL_TouchscreenPoint(150, 190, 0);

    int16_t illpieangles[] = { 60, 0, 120, 150 };
    EXPECT_EQ(ILL_ARG_ANGLE, ete.isInPieSlice(p, 100, 200, 60, 4, illpieangles));

    int16_t pieanglesGT360[] = { 70, 30, 120, 150 };
    EXPECT_EQ(ILL_ARG_SUM_ANGLE, ete.isInPieSlice(p, 100, 200, 60, 4, pieanglesGT360));

}
//#####################################################################

// 
#define X_OFFSET 9
#define Y_OFFSET 4

TEST(test_ExtendedTouchEvent, test_isInPolygon) {
    int16_t vert[][2] = {{10, 10}, {15, 15}, {15, 10}, {20, 12}, {12, 5}};
    const char *data[] = {
      "             ", // effective Y = 4 (= Y_OFFSET)
      "   .         ",
      "   ..        ",
      "   ...       ",
      "  .....      ",
      "  ......     ",
      " ........    ",
      "  ....   .   ",
      "   ...     . ",
      "    ..       ",
      "     .       ",
      "      .      ",
      "             "
    };

    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);
    for (int y = 0; y < 13; y++) {
        const char *row = data[y];
        for (int x = 0; x < 12; x++) {
            char mes[20];
            snprintf(mes, 20, "x = %d, y = %d", x, y);

            auto p1 = HAL_TouchscreenPoint(x + X_OFFSET, y + Y_OFFSET, 8);
            EXPECT_EQ(row[x] == '.', ete.isInPolygon(p1, 5, vert)) << mes;
        }
    }

    auto p1 = HAL_TouchscreenPoint(20, 30, 8);
    EXPECT_FALSE(ete.isInPolygon(p1, 5, vert));
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_setResolution) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft);

    ete.setResolution(120, 160);
    //?? assertion????
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenSimpleTouch) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDown(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);

    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) {ts_event(2, 10, 0, EVENT::EVT_DOWN)});
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(2, 10, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenSimpleTouchWithoutAll) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnTouchDown(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);

    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) {});
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(2, 10, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenContinuousTouch) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;

    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 460, 3835, 25, 15L);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) {ts_event(2, 10, 0, EVENT::EVT_DOWN)});
    ASSERT_EQ(ete.lastTouchEventTime(), 15L);
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenContinuousLongTouch) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchLong(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);
    // 2nd poll (with slightly different y coordinate, but no move) - long time
    preparePoll(seq, ts, tft, compat, 460, 3835, 25, 1011L);


    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(2, 10, 0, EVENT::EVT_DOWN),
                                                                 ts_event(2, 10, 0, EVENT::EVT_LONG) });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(2, 10, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenDraw) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDraw(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll0
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);
    // 2nd poll (with different x coordinate - move)
    preparePoll(seq, ts, tft, compat, 600, 3830, 20, 111);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(2, 10, 0, EVENT::EVT_DOWN),
                                                                 ts_event(2, 23, 0, EVENT::EVT_DRAW) });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(2, 23, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenDrawMoveToSmall) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDraw(event_handler_for_tests::ts_Point_callback);
    ete.setMoveTreshold(200);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll0
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);
    // 2nd poll (with different x coordinate - move, but moved too less)
    preparePoll(seq, ts, tft, compat, 600, 3830, 20, 111);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(2, 10, 0, EVENT::EVT_DOWN) });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {} );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenNoDraw) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDraw(event_handler_for_tests::ts_Point_callback);
    ete.setDrawMode(false);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll0
    preparePoll(seq, ts, tft, compat, 460, 3830, 20, 10L);
    // 2nd poll (with different x coordinate - move)
    preparePoll(seq, ts, tft, compat, 600, 3830, 20, 111);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(2, 10, 0, EVENT::EVT_DOWN) });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {} );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenClick) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchClick(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 470, 3820, 20, 10L);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 480, 3800, 2, 511);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(3, 11, 0, EVENT::EVT_DOWN),
                                                                 ts_event(4, 12, 0, EVENT::EVT_UP),
                                                                 ts_event(4, 12, 0, EVENT::EVT_CLICK)
                                                                });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(4, 12, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenUp) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchUp(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 470, 3820, 20, 10L);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 480, 3800, 2, 511);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(3, 11, 0, EVENT::EVT_DOWN),
                                                                 ts_event(4, 12, 0, EVENT::EVT_UP),
                                                                 ts_event(4, 12, 0, EVENT::EVT_CLICK)
                                                                });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(4, 12, 0) } );
}
//#####################################################################

#ifndef ESP8266
// ESP8266 crashes with OOM - may be too complex?
TEST(test_ExtendedTouchEvent, test_pollTouchScreenLongClick) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchLong(event_handler_for_tests::ts_Point_callback);

    // shorter time for long click
    ete.setLongClick(400L);
    // no draw mode
    ete.setDrawMode(false);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 470, 3820, 20, 10L);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 480, 3800, 20, 511);
    // 3rd poll (with slightly different y coordinate, but still down)
    preparePoll(seq, ts, tft, compat, 480, 3800, 20, 611);
    // 4th poll (with slightly different y coordinate, now up)
    preparePoll(seq, ts, tft, compat, 480, 3810, 2, 711);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();
    // 3rd poll
    ete.pollTouchScreen();
    // 4th poll
    ete.pollTouchScreen();

    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(3, 11, 0, EVENT::EVT_DOWN),
                                                                 ts_event(4, 12, 0, EVENT::EVT_LONG),
                                                                 ts_event(3, 12, 0, EVENT::EVT_UP)
                                                                });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(4, 12, 0) } );
}
#endif
//#####################################################################

// ESP8266 crashes with OOM - may be too complex?
#ifndef ESP8266

// 4 tft-based raw coordinates for test_pollTouchScreenWipe for each rotation
static const int16_t TFT_RAWS[][4] = {
    {  470, 3820,  480, 3110 },  // ROTATION::PORTRAIT_TR
    {  390,  400,  900,  410 },  // ROTATION::LANDSCAPE_BR
    {  280, 3420,  990, 3410 },  // ROTATION::PORTRAIT_BL
    { 3505, 3695, 2995, 3685 }   // ROTATION::LANDSCAPE_TL
};

// this test runs parametrized
TEST_P(WipeTestSuite, test_pollTouchScreenWipe) {
    printf("################################ rotation=%hu ###################################\n", GetParam().rot);
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);
    // smaller wipe distance
    ete.setWipe(500, 500);

    event_handler_for_tests handler;

    ete.setDrawMode(false); // no wipe-events in drawmode
    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchWipe(event_handler_for_tests::wipe_callback);
    // Parameter comes from gtest
    auto rot = GetParam().rot;
    auto reverse = GetParam().reverse;
    auto vertical = GetParam().vertical;

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // 1st of all - setting the rotation
    EXPECT_CALL(ts, setRotation(Matcher<ROTATION>(Eq(rot)))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(tft, setRotation(Matcher<ROTATION>(Eq(rot)))).InSequence(seq).RetiresOnSaturation();

    const int16_t *params = TFT_RAWS[(int) rot];

    if (reverse) {
        if (vertical) {
            // prepare 1st poll
            preparePoll(seq, ts, tft, compat, params[3], params[2], 20, 511, rot);
            // 2nd poll (with different x coordinate - wipe)
            preparePoll(seq, ts, tft, compat, params[1], params[0], 2, 10, rot);

            // two additional calls of toScreen()
            auto raw = toRaw(rot, params[3], params[2]);
            mockToScreen(seq, tft, compat, ts, raw.x, raw.y, 20, 511, rot);  // raw data of 1nd poll but actual millis
            auto raw2 = toRaw(rot, params[1], params[0]);
            mockToScreen(seq, tft, compat, ts, raw2.x, raw2.y, 2, 511, rot); // raw2 data of 2st poll but actual millis
        } else {
            // prepare 1st poll
            preparePoll(seq, ts, tft, compat, params[2], params[3], 20, 511, rot);
            // 2nd poll (with different x coordinate - wipe)
            preparePoll(seq, ts, tft, compat, params[0], params[1], 2, 10, rot);

            // two additional calls of toScreen()
            auto raw = toRaw(rot, params[2], params[3]);
            mockToScreen(seq, tft, compat, ts, raw.x, raw.y, 20, 511, rot);  // raw data of 1nd poll but actual millis
            auto raw2 = toRaw(rot, params[0], params[1]);
            mockToScreen(seq, tft, compat, ts, raw2.x, raw2.y, 2, 511, rot); // raw2 data of 2st poll but actual millis
        }
    } else {
        if (vertical) {
            // prepare 1st poll
            preparePoll(seq, ts, tft, compat, params[1], params[0], 20, 10, rot);
            // 2nd poll (with different x coordinate - wipe)
            preparePoll(seq, ts, tft, compat, params[3], params[2], 2, 511, rot);

            // two additional calls of toScreen()
            auto raw = toRaw(rot, params[1], params[0]);
            mockToScreen(seq, tft, compat, ts, raw.x, raw.y,  20, 511, rot); // raw data of 1st poll but actual millis
            auto raw2 = toRaw(rot, params[3], params[2]);
            mockToScreen(seq, tft, compat, ts, raw2.x, raw2.y,  2, 511, rot);  // raw data of 2nd poll but actual millis
        } else {
            // prepare 1st poll
            preparePoll(seq, ts, tft, compat, params[0], params[1], 20, 10, rot);
            // 2nd poll (with different x coordinate - wipe)
            preparePoll(seq, ts, tft, compat, params[2], params[3], 2, 511, rot);

            // two additional calls of toScreen()
            auto raw = toRaw(rot, params[0], params[1]);
            mockToScreen(seq, tft, compat, ts, raw.x, raw.y,  20, 511, rot); // raw data of 1st poll but actual millis
            auto raw2 = toRaw(rot, params[2], params[3]);
            mockToScreen(seq, tft, compat, ts, raw2.x, raw2.y,  2, 511, rot);  // raw data of 2nd poll but actual millis
        }
    }


    // setting the rotation
    ete.setRotation(rot);
   // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();

    // assertions
    if (reverse) {
        if (vertical) {
            event_handler_for_tests::checkEvents(std::list<ts_event> { ts_event(5, 46, 0, EVENT::EVT_DOWN),
                                                                    ts_event(4, 11, 0, EVENT::EVT_UP),
                                                                    ts_event(4, 11, 0, EVENT::EVT_WIPE)
            });
        } else  {
            event_handler_for_tests::checkEvents(std::list<ts_event> { ts_event(52, 12, 0, EVENT::EVT_DOWN),
                                                                    ts_event(3, 11, 0, EVENT::EVT_UP),
                                                                    ts_event(3, 11, 0, EVENT::EVT_WIPE)
            });
        }
    } else {
        if (vertical) {
            event_handler_for_tests::checkEvents(std::list<ts_event> { ts_event(4, 11, 0, EVENT::EVT_DOWN),
                                                                       ts_event(5, 46, 0, EVENT::EVT_UP),
                                                                       ts_event(5, 46, 0, EVENT::EVT_WIPE)
            });
        } else {
            event_handler_for_tests::checkEvents(std::list<ts_event> { ts_event(3, 11, 0, EVENT::EVT_DOWN),
                                                                       ts_event(52, 12, 0, EVENT::EVT_UP),
                                                                       ts_event(52, 12, 0, EVENT::EVT_WIPE)
            });
        }
    }

    event_handler_for_tests::checkEvents(std::list<WIPE_DIRECTION> { GetParam().dir } );
}
#endif
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreenDblClick) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;

    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchDblClick(event_handler_for_tests::ts_Point_callback);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 470, 3820, 20, 10);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 480, 3800, 2, 511);
    // prepare 3rd poll
    preparePoll(seq, ts, tft, compat, 480, 3800, 20, 700);
    // 4th
    preparePoll(seq, ts, tft, compat, 490, 3800, 2, 1000);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();
    // 3rd poll
    ete.pollTouchScreen();
    // 4th poll
    ete.pollTouchScreen();
    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(3, 11, 0, EVENT::EVT_DOWN)
                                                               , ts_event(4, 12, 0, EVENT::EVT_UP)
                                                               , ts_event(4, 12, 0, EVENT::EVT_CLICK)
                                                               , ts_event(4, 12, 0, EVENT::EVT_DOWN)
                                                               , ts_event(4, 13, 0, EVENT::EVT_UP)
                                                               , ts_event(4, 13, 0, EVENT::EVT_DBLCLICK)
    });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {HAL_TouchscreenPoint(4, 13, 0) } );
}
//#####################################################################

TEST(test_ExtendedTouchEvent, test_pollTouchScreen2Clicks) {
    Mock_XPT2046_Touchscreen ts;
    Mock_TFT tft;
    Mock_Compat compat;

    ExtendedTouchEvent ete = ExtendedTouchEvent(ts, tft, compat);

    event_handler_for_tests handler;

    ete.registerOnAllEvents(event_handler_for_tests::callback_allevents);
    ete.registerOnTouchClick(event_handler_for_tests::ts_Point_callback);
    // less time for double click
    ete.setDblClick(300);

    //without that gMock will search the expectations in the reverse order they are defined
    const Sequence seq;

    // prepare 1st poll
    preparePoll(seq, ts, tft, compat, 470, 3820, 20, 10);
    // 2nd poll (with slightly different y coordinate, but no move)
    preparePoll(seq, ts, tft, compat, 480, 3800, 2, 511);
    // prepare 3rd poll
    preparePoll(seq, ts, tft, compat, 480, 3800, 20, 700);
    // 4th
    preparePoll(seq, ts, tft, compat, 490, 3800, 2, 1000);

    // 1st poll
    ete.pollTouchScreen();
    // 2nd poll
    ete.pollTouchScreen();
    // 3rd poll
    ete.pollTouchScreen();
    // 4th poll
    ete.pollTouchScreen();
    // assertions
    event_handler_for_tests::checkEvents((std::list<ts_event>) { ts_event(3, 11, 0, EVENT::EVT_DOWN)
                                                               , ts_event(4, 12, 0, EVENT::EVT_UP)
                                                               , ts_event(4, 12, 0, EVENT::EVT_CLICK)
                                                               , ts_event(4, 12, 0, EVENT::EVT_DOWN)
                                                               , ts_event(4, 13, 0, EVENT::EVT_UP)
                                                               , ts_event(4, 13, 0, EVENT::EVT_CLICK)
    });
    event_handler_for_tests::checkEvents(std::list<HAL_TouchscreenPoint> {
        HAL_TouchscreenPoint(4, 12, 0), 
        HAL_TouchscreenPoint(4, 13, 0) 
    } );
}
//#####################################################################

#ifdef NATIVE
    int main(int n, char ** args) {
    testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}
#else
    #include "Arduino.h"

void setup() {
    Serial.begin(SPEED);
    delay(1000);
    testing::InitGoogleTest();
    if (RUN_ALL_TESTS()) delay(50);
}

void loop() {
    // nothing to be done here.
    delay(100);
}
#endif