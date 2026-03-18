import type { AiOperation, AiOpsDraft, ValidationResult } from './schema';

function isObject(value: unknown): value is Record<string, unknown> {
  return Boolean(value) && typeof value === 'object' && !Array.isArray(value);
}

function isValidOperation(value: unknown): value is AiOperation {
  if (!isObject(value) || typeof value.type !== 'string') {
    return false;
  }

  switch (value.type) {
    case 'addControl':
      return isObject(value.control) && typeof value.control.type === 'string';
    case 'updateControl':
      return typeof value.target === 'string' && isObject(value.updates);
    case 'removeControl':
    case 'renameControl':
    case 'moveControl':
    case 'resizeControl':
    case 'reparentControl':
    case 'createEventPlaceholder':
      return typeof value.target === 'string';
    case 'updateWindow':
      return isObject(value.updates);
    default:
      return false;
  }
}

export function validateOps(input: unknown): ValidationResult<AiOpsDraft> {
  if (!isObject(input)) {
    return { ok: false, errors: ['Ops 必须是对象'] };
  }
  if (!Array.isArray(input.operations) || input.operations.some((operation) => !isValidOperation(operation))) {
    return { ok: false, errors: ['Ops.operations 格式不正确'] };
  }

  return {
    ok: true,
    data: {
      operations: input.operations,
      affectedControls: Array.isArray(input.affectedControls)
        ? input.affectedControls.filter((item): item is string => typeof item === 'string')
        : [],
      summary: typeof input.summary === 'string' ? input.summary : '',
      warnings: Array.isArray(input.warnings) ? input.warnings.filter((item): item is string => typeof item === 'string') : [],
    },
  };
}
