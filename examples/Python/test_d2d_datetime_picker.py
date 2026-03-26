# -*- coding: utf-8 -*-
"""D2D 日期时间选择器 API 自动化测试（需 64 位 Python + x64 emoji_window.dll）。"""
from __future__ import annotations

import ctypes
import unittest
from ctypes import wintypes

import emoji_window_dll as ew


class TestD2DDateTimePicker(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.dll = ew.load_emoji_window_dll()
        ew.setup_datetime_picker_functions(cls.dll)
        cls.user32 = ctypes.windll.user32

    def setUp(self):
        # 与目录下其它 test_*.py 共用进程时，其它模块可能改写 DLL 函数签名
        ew.setup_datetime_picker_functions(self.dll)

    def _create_window(self):
        ew.setup_datetime_picker_functions(self.dll)
        h = ew.create_window_utf8(self.dll, "Python DTP 测试", -1, -1, 640, 480)
        self.assertIsNotNone(h)
        self.assertNotEqual(ctypes.cast(h, ctypes.c_void_p).value, 0)
        return h

    def _create_picker(self, parent, precision=ew.DTP_YMD):
        ew.setup_datetime_picker_functions(self.dll)
        font = ew.utf8_bytes("Microsoft YaHei UI")
        fbuf = (ctypes.c_ubyte * len(font))(*font)
        h = self.dll.CreateD2DDateTimePicker(
            parent,
            24,
            56,
            340,
            36,
            precision,
            0xFF303133,
            0xFFFFFFFF,
            0xFFDCDFE6,
            ctypes.cast(fbuf, ctypes.c_void_p),
            len(font),
            14,
            0,
            0,
            0,
        )
        self.assertIsNotNone(h)
        self.assertNotEqual(ctypes.cast(h, ctypes.c_void_p).value, 0)
        return h

    def _get_dt(self, picker):
        y = ctypes.c_int()
        mo = ctypes.c_int()
        d = ctypes.c_int()
        hh = ctypes.c_int()
        mm = ctypes.c_int()
        ss = ctypes.c_int()
        self.dll.GetD2DDateTimePickerDateTime(
            picker,
            ctypes.byref(y),
            ctypes.byref(mo),
            ctypes.byref(d),
            ctypes.byref(hh),
            ctypes.byref(mm),
            ctypes.byref(ss),
        )
        return (
            y.value,
            mo.value,
            d.value,
            hh.value,
            mm.value,
            ss.value,
        )

    def test_precision_get_set_and_normalize_ymd(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMD)
            self.assertEqual(self.dll.GetD2DDateTimePickerPrecision(p), ew.DTP_YMD)
            self.dll.SetD2DDateTimePickerDateTime(p, 2024, 6, 15, 14, 30, 45)
            t = self._get_dt(p)
            self.assertEqual(t, (2024, 6, 15, 0, 0, 0))

            self.dll.SetD2DDateTimePickerPrecision(p, ew.DTP_YMDHMS)
            self.assertEqual(self.dll.GetD2DDateTimePickerPrecision(p), ew.DTP_YMDHMS)
            self.dll.SetD2DDateTimePickerDateTime(p, 2025, 3, 26, 9, 8, 7)
            self.assertEqual(self._get_dt(p), (2025, 3, 26, 9, 8, 7))

            self.dll.SetD2DDateTimePickerPrecision(p, ew.DTP_YMD)
            t2 = self._get_dt(p)
            self.assertEqual(t2[0:3], (2025, 3, 26))
            self.assertEqual(t2[3:], (0, 0, 0))
        finally:
            self.dll.destroy_window(wnd)

    def test_callback_on_set_datetime(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMD)
            seen = []

            def cb(h):
                seen.append(h)

            cb_keep = self.dll._ValueChangedCallback(cb)
            self.dll.SetD2DDateTimePickerCallback(p, cb_keep)
            self.dll.SetD2DDateTimePickerDateTime(p, 2020, 1, 2, 0, 0, 0)
            self.assertEqual(len(seen), 1)
            self.assertEqual(seen[0], p)
        finally:
            self.dll.destroy_window(wnd)

    def test_enable_show_bounds(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMD)
            self.dll.EnableD2DDateTimePicker(p, 0)
            self.assertEqual(self.user32.IsWindowEnabled(p), 0)
            self.dll.EnableD2DDateTimePicker(p, 1)
            self.assertNotEqual(self.user32.IsWindowEnabled(p), 0)

            self.dll.ShowD2DDateTimePicker(p, 0)
            self.assertEqual(self.user32.IsWindowVisible(p), 0)
            self.dll.ShowD2DDateTimePicker(p, 1)
            self.assertNotEqual(self.user32.IsWindowVisible(p), 0)

            self.dll.SetD2DDateTimePickerBounds(p, 10, 20, 400, 40)
            # GetWindowBounds/GetWindowRect 为屏幕坐标；与父窗口客户区 (10,20) 用 MapWindowPoints 对照
            pt = wintypes.POINT(10, 20)
            self.user32.MapWindowPoints(wnd, None, ctypes.byref(pt), 1)
            r = wintypes.RECT()
            self.user32.GetWindowRect(p, ctypes.byref(r))
            self.assertEqual(r.left, pt.x)
            self.assertEqual(r.top, pt.y)
            self.assertEqual(r.right - r.left, 400)
            self.assertEqual(r.bottom - r.top, 40)
        finally:
            self.dll.destroy_window(wnd)

    def test_year_precision(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YEAR)
            self.assertEqual(self.dll.GetD2DDateTimePickerPrecision(p), ew.DTP_YEAR)
            self.dll.SetD2DDateTimePickerDateTime(p, 2023, 6, 15, 12, 0, 0)
            y, mo, d, hh, mm, ss = self._get_dt(p)
            self.assertEqual(y, 2023)
            self.assertEqual((mo, d), (1, 1))
            self.assertEqual((hh, mm, ss), (0, 0, 0))
        finally:
            self.dll.destroy_window(wnd)

    def test_precision_ymdh_normalizes_minute_second(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMDH)
            self.dll.SetD2DDateTimePickerDateTime(p, 2024, 5, 1, 13, 44, 55)
            self.assertEqual(self._get_dt(p), (2024, 5, 1, 13, 0, 0))
        finally:
            self.dll.destroy_window(wnd)

    def test_precision_ymdhm_normalizes_second(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMDHM)
            self.dll.SetD2DDateTimePickerDateTime(p, 2024, 5, 1, 13, 44, 55)
            self.assertEqual(self._get_dt(p), (2024, 5, 1, 13, 44, 0))
        finally:
            self.dll.destroy_window(wnd)

    def test_callback_on_precision_change(self):
        wnd = self._create_window()
        try:
            p = self._create_picker(wnd, ew.DTP_YMD)
            n = []

            def cb(h):
                n.append(h)

            cb_keep = self.dll._ValueChangedCallback(cb)
            self.dll.SetD2DDateTimePickerCallback(p, cb_keep)
            self.dll.SetD2DDateTimePickerPrecision(p, ew.DTP_YMDHMS)
            self.assertGreaterEqual(len(n), 1)
            self.assertEqual(n[-1], p)
        finally:
            self.dll.destroy_window(wnd)


if __name__ == "__main__":
    unittest.main(verbosity=2)
