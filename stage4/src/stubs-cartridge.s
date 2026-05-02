| Copyright (c) 2026 Clownacy

| Permission to use, copy, modify, and/or distribute this software for any
| purpose with or without fee is hereby granted.

| THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
| REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
| AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
| INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
| LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
| OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
| PERFORMANCE OF THIS SOFTWARE.

	.section  .text
	.weak _BusErrorHandler
	.weak _AddressErrorHandler
	.weak _IllegalInstructionHandler
	.weak _DivisionByZeroHandler
	.weak _CHKHandler
	.weak _TRAPVHandler
	.weak _PrivilegeViolationHandler
	.weak _TraceHandler
	.weak _UnimplementedInstructionLineAHandler
	.weak _UnimplementedInstructionLineFHandler
	.weak _UnassignedHandler
	.weak _UninitialisedInterruptHandler
	.weak _SpuriousInterruptHandler
	.weak _ControllerInterruptHandler
	.weak _HorizontalInterruptHandler
	.weak _VerticalInterruptHandler
	.weak _TRAP0Handler
	.weak _TRAP1Handler
	.weak _TRAP2Handler
	.weak _TRAP3Handler
	.weak _TRAP4Handler
	.weak _TRAP5Handler
	.weak _TRAP6Handler
	.weak _TRAP7Handler
	.weak _TRAP8Handler
	.weak _TRAP9Handler
	.weak _TRAP10Handler
	.weak _TRAP11Handler
	.weak _TRAP12Handler
	.weak _TRAP13Handler
	.weak _TRAP14Handler
	.weak _TRAP15Handler

| Fallbacks; can be overridden by application code to hook these.
_BusErrorHandler:
_AddressErrorHandler:
_IllegalInstructionHandler:
_DivisionByZeroHandler:
_CHKHandler:
_TRAPVHandler:
_PrivilegeViolationHandler:
_TraceHandler:
_UnimplementedInstructionLineAHandler:
_UnimplementedInstructionLineFHandler:
_UnassignedHandler:
_UninitialisedInterruptHandler:
_SpuriousInterruptHandler:
_ControllerInterruptHandler:
_HorizontalInterruptHandler:
_VerticalInterruptHandler:
_TRAP0Handler:
_TRAP1Handler:
_TRAP2Handler:
_TRAP3Handler:
_TRAP4Handler:
_TRAP5Handler:
_TRAP6Handler:
_TRAP7Handler:
_TRAP8Handler:
_TRAP9Handler:
_TRAP10Handler:
_TRAP11Handler:
_TRAP12Handler:
_TRAP13Handler:
_TRAP14Handler:
_TRAP15Handler:
	rte
