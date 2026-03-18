const { addDays, addMonths, addYears, subDays, getDay, isSameDay, format, parseISO, isValid } = require('date-fns');

const recurringPeriodList = ['', 'Week', 'Fortnight', 'Month', 'Quarter', 'Half Year', 'Year'];

function setNextDatePeriod(currentDate, periodID) {
    const d = currentDate instanceof Date ? currentDate : new Date(currentDate);
    const id = String(periodID);
    switch (id) {
        case '1': return addDays(d, 7);
        case '2': return addDays(d, 14);
        case '3': return addMonths(d, 1);
        case '4': return addMonths(d, 3);
        case '5': return addMonths(d, 6);
        case '6': return addYears(d, 1);
        default: return d;
    }
}

function getDayOfWeekDate(dtDate, dtDayOfWeek) {
    const d = dtDate instanceof Date ? new Date(dtDate) : new Date(dtDate);
    const day = getDay(d);
    if (day >= dtDayOfWeek) {
        return subDays(d, day - dtDayOfWeek);
    } else {
        return subDays(d, 7 - (dtDayOfWeek - day));
    }
}

function toNullable(value) {
    if (value === '' || value === null || value === undefined || value === '0' || value === 'null') {
        return null;
    }
    return value;
}

function toNullableNumber(value) {
    if (value === '' || value === null || value === undefined || value === '0' || value === 'null' || isNaN(value)) {
        return null;
    }
    return value;
}

function formatDate(date) {
    if (!date) return null;
    const d = date instanceof Date ? date : new Date(date);
    if (!isValid(d)) return null;
    return format(d, 'yyyy-MM-dd');
}

module.exports = { recurringPeriodList, setNextDatePeriod, getDayOfWeekDate, toNullable, toNullableNumber, formatDate };
