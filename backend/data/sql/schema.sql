DROP TABLE IF EXISTS `InternalEvents`;
DROP TABLE IF EXISTS `RejectEvents`;
DROP TABLE IF EXISTS `IncomingMessageEvents`;
DROP TABLE IF EXISTS `Orders`;
DROP TABLE IF EXISTS `Strategies`;
DROP TABLE IF EXISTS `StrategyStatuses`;
DROP TABLE IF EXISTS `Instruments`;
DROP TABLE IF EXISTS `OrderTypes`;
DROP TABLE IF EXISTS `EventTypes`;
DROP TABLE IF EXISTS `Backends`;
DROP TABLE IF EXISTS `IDGenerator`;

create table `IDGenerator` (
   `id` BIGINT UNSIGNED NOT NULL)
 ENGINE=InnoDB;

CREATE TABLE `Orders` (
  `Id` BIGINT UNSIGNED NOT NULL,
  `ClientOrderId` VARCHAR(750) NOT NULL,
  `StrategyId` BIGINT UNSIGNED NOT NULL,
  `BackendId` BIGINT UNSIGNED NOT NULL,
  `EventType` INT UNSIGNED NOT NULL,
  `OrderType` ENUM('Primary','Secondary_Limit','Secondary_Stop','OrderType_MAX') NOT NULL,
  `SentQuantity` DOUBLE PRECISION(20, 10) NOT NULL,
  `SentPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `Side` ENUM('BUY', 'SELL', 'OrderMode_MAX') NOT NULL,
  `Timestamp` TIMESTAMP(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
   PRIMARY KEY (`ClientOrderId`,`BackendId`))
 ENGINE=InnoDB;

CREATE TABLE `InternalEvents` (
  `Id` BIGINT UNSIGNED NOT NULL PRIMARY KEY,
  `StrategyId` BIGINT UNSIGNED NOT NULL,
  `BackendId` BIGINT UNSIGNED NOT NULL,
  `EventType` INT UNSIGNED NOT NULL,
  `EventPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `Timestamp` TIMESTAMP(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6))
 ENGINE=InnoDB;

CREATE TABLE `Instruments` (
  `Id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  `Name` TEXT NOT NULL,
  `SecType` TEXT NOT NULL,
  `Exchange` TEXT NOT NULL,
  `Currency` TEXT NOT NULL,
  `Type` TEXT NOT NULL)
 ENGINE=InnoDB;

CREATE TABLE `Strategies` (
  `StrategyId` BIGINT UNSIGNED NOT NULL,
  `BackendId` BIGINT UNSIGNED NOT NULL,
  `InternalId` BIGINT UNSIGNED NOT NULL,
  `UploadId` TEXT NOT NULL,
  `InstrumentId` BIGINT UNSIGNED NOT NULL,
  `TickSize` DOUBLE PRECISION(20, 10) NOT NULL,
  `AlgoType` INT UNSIGNED NOT NULL,
  `Side` ENUM('BUY', 'SELL', 'OrderMode_MAX') NOT NULL,
  `PreviousClose` DOUBLE PRECISION(20, 10) NOT NULL,
  `BaseValue` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointAPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointAPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointBPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointBPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointCPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `PointCPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `CP1Price` DOUBLE PRECISION(20, 10) NOT NULL,
  `CP1Percentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `CP2Price` DOUBLE PRECISION(20, 10) NOT NULL,
  `CP2Percentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `ParentOrderPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `Profit` DOUBLE PRECISION(20, 10) NOT NULL,
  `ProfitPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `Loss` DOUBLE PRECISION(20, 10) NOT NULL,
  `LossPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `LossLimitPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `Trailing` DOUBLE PRECISION(20, 10) NOT NULL,
  `TrailingPercentage` DOUBLE PRECISION(20, 10) NOT NULL,
  `Tolerance` DOUBLE PRECISION(20, 10) NOT NULL,
  `StartDate` DATE NULL,
  `StartTime` TIME NULL,
  `EndDate` DATE NULL,
  `EndTime` TIME NULL,
  `TExitDate` DATE NULL,
  `TExitTime` TIME NULL,
  `Quantity` DOUBLE PRECISION(20, 10) NOT NULL,
  `CancelTicks` INT NOT NULL,
  `UploadName` TEXT NOT NULL,
  `StrategyStatus` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`StrategyId`,
               `BackendId`))
 ENGINE=InnoDB;

CREATE TABLE `RejectEvents` (
  `Id` BIGINT UNSIGNED NOT NULL PRIMARY KEY,
  `ClientOrderId` VARCHAR(750) NOT NULL,
  `BackendId` BIGINT UNSIGNED NOT NULL,
  `Reason` VARCHAR(2048) NOT NULL,
  `Timestamp` TIMESTAMP(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6))
 ENGINE=InnoDB;

CREATE TABLE `Backends` (
  `Id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  `Uuid` VARCHAR(36) NOT NULL,
  `BuyAccount` VARCHAR(1024) NOT NULL,
  `SellAccount` VARCHAR(1024) NOT NULL,
  `DataFeedVenueId` VARCHAR(1024) NOT NULL,
  `OrderVenueId` VARCHAR(1024) NOT NULL)
 ENGINE=InnoDB;

CREATE TABLE `EventTypes` (
  `Id` INT UNSIGNED NOT NULL PRIMARY KEY,
  `Name` TEXT NOT NULL)
 ENGINE=InnoDB;

CREATE TABLE `OrderTypes` (
  `Id` INT UNSIGNED NOT NULL PRIMARY KEY,
  `Name` TEXT NOT NULL)
 ENGINE=InnoDB;

DROP TABLE IF EXISTS `IncomingMessageEvents`;

CREATE TABLE `IncomingMessageEvents` (
  `Id` BIGINT UNSIGNED NOT NULL PRIMARY KEY,
  `ClientOrderId` varchar(750) NOT NULL,
  `BackendId` BIGINT UNSIGNED NOT NULL,
  `ExchangeOrderId` VARCHAR(1024) NOT NULL,
  `EventType` INT UNSIGNED NOT NULL,
  `ReceivedQuantity` DOUBLE PRECISION(20, 10) NOT NULL,
  `ReceivedPrice` DOUBLE PRECISION(20, 10) NOT NULL,
  `Timestamp` TIMESTAMP(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6))
 ENGINE=InnoDB;

CREATE TABLE `StrategyStatuses` (
  `Id` INT UNSIGNED NOT NULL PRIMARY KEY,
  `Name` TEXT NOT NULL)
 ENGINE=InnoDB;


ALTER TABLE `Orders`
  ADD CONSTRAINT `Orders_strategy_fk`
    FOREIGN KEY (`StrategyId`,
                 `BackendId`)
    REFERENCES `Strategies` (`StrategyId`,
                             `BackendId`),
  ADD CONSTRAINT `Orders_EventType_fk`
    FOREIGN KEY (`EventType`)
    REFERENCES `EventTypes` (`Id`);

ALTER TABLE `InternalEvents`
  ADD CONSTRAINT `InternalEvents_strategy_fk`
    FOREIGN KEY (`StrategyId`,
                 `BackendId`)
    REFERENCES `Strategies` (`StrategyId`,
                             `BackendId`),
  ADD CONSTRAINT `InternalEvents_EventType_fk`
    FOREIGN KEY (`EventType`)
    REFERENCES `EventTypes` (`Id`);

ALTER TABLE `Strategies`
  ADD CONSTRAINT `Strategies_BackendId_fk`
    FOREIGN KEY (`BackendId`)
    REFERENCES `Backends` (`Id`),
  ADD CONSTRAINT `Strategies_InstrumentId_fk`
    FOREIGN KEY (`InstrumentId`)
    REFERENCES `Instruments` (`Id`),
  ADD CONSTRAINT `Strategies_StrategyStatus_fk`
    FOREIGN KEY (`StrategyStatus`)
    REFERENCES `StrategyStatuses` (`Id`);

ALTER TABLE `IncomingMessageEvents`
  ADD CONSTRAINT `IncomingMessageEvents_ClientOrderId_fk`
    FOREIGN KEY (`ClientOrderId`,`BackendId`)
    REFERENCES `Orders` (`ClientOrderId`,`BackendId`),
  ADD CONSTRAINT `IncomingMessageEvents_EventType_fk`
    FOREIGN KEY (`EventType`)
    REFERENCES `EventTypes` (`Id`);

ALTER TABLE `RejectEvents`
  ADD CONSTRAINT `RejectEvents_ClientOrderId_fk`
    FOREIGN KEY (`ClientOrderId`,`BackendId`)
    REFERENCES `Orders` (`ClientOrderId`,`BackendId`);

#
# INSERT DEFAULT VALUES
#
INSERT INTO EventTypes VALUES  (1, "Point A"), (2, "Point B"), (3, "Point C"), (4, "ACK"), (5, "FILLED"), (6, "ACTIVE"), (7, "CANCELED"), (8, "REJECTED"), (9, "REJECTED Secondary Limit Modify"), (10, "CANCEL POINT 1"), (11, "CANCEL POINT 2"), (12, "T-Exit Limit Replacement"), (13, "T-EXIT Ack Secondary Limit"), (14, "Strategy Changed Pending to Running"), (15, "Strategy Terminated due to Stop Time"), (16, "Error Sending IOC"), (17, "Cancel Tick A"), (18, "Point A Reached Before Start"), (19, "Max Re-attempt Reached"), (20, "Error Modifying Limit T-Exit"), (21, "Bracket Order"), (22, "Strategy Terminated from GUI"), (23, "Cancel Tick B"), (24, "Killed MaxOpen");


INSERT INTO OrderTypes VALUES (1, "Primary"), (2, "Secondary Stop"), (3, "Secondary Limit");

INSERT INTO StrategyStatuses VALUES (1, "PENDING"), (2, "WAITING"), (3, "OPEN"), (4, "STOPPED"), (5, "STARTED"), (6, "RUNNING"), (7, "UNKNOWN");

INSERT INTO IDGenerator VALUES (0);


DELIMITER //
DROP TRIGGER IF EXISTS IncomingMessageEvents_before_insert //
DROP TRIGGER IF EXISTS RejectEvents_before_insert //
DROP TRIGGER IF EXISTS Orders_before_insert //
DROP TRIGGER IF EXISTS InternalEvents_before_insert //
CREATE
TRIGGER IncomingMessageEvents_before_insert
    BEFORE INSERT ON
        IncomingMessageEvents
    FOR EACH ROW
BEGIN
    UPDATE IDGenerator SET id=LAST_INSERT_ID(id+1);
    SET NEW.Id = LAST_INSERT_ID();
END;
//
CREATE
TRIGGER RejectEvents_before_insert
    BEFORE INSERT ON
        RejectEvents
    FOR EACH ROW
BEGIN
    UPDATE IDGenerator SET id=LAST_INSERT_ID(id+1);
    SET NEW.Id = LAST_INSERT_ID();
END;
//
CREATE
TRIGGER Orders_before_insert
    BEFORE INSERT ON
        Orders
    FOR EACH ROW
BEGIN
    UPDATE IDGenerator SET id=LAST_INSERT_ID(id+1);
    SET NEW.Id = LAST_INSERT_ID();
END;
//
CREATE
TRIGGER InternalEvents_before_insert
    BEFORE INSERT ON
        InternalEvents
    FOR EACH ROW
BEGIN
    UPDATE IDGenerator SET id=LAST_INSERT_ID(id+1);
    SET NEW.Id = LAST_INSERT_ID();
END;
//

DELIMITER ;
DROP FUNCTION IF EXISTS `decimal_trim`;
create FUNCTION decimal_trim ( s DOUBLE(20, 10)) RETURNS double
return trim(TRAILING '.' from trim(TRAILING '0' FROM s));

