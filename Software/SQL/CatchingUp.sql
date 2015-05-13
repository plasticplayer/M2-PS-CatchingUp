-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb2+deb7u1
-- http://www.phpmyadmin.net
--
-- Client: localhost
-- Généré le: Mer 13 Mai 2015 à 13:13
-- Version du serveur: 5.5.41
-- Version de PHP: 5.4.39-0+deb7u2

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de données: `CatchingUp`
--

-- --------------------------------------------------------

--
-- Structure de la table `Attachment`
--

CREATE TABLE IF NOT EXISTS `Attachment` (
  `IdAttachment` int(11) NOT NULL AUTO_INCREMENT,
  `IdLesson` int(11) NOT NULL,
  `NameAttachment` varchar(45) NOT NULL,
  `PathAttachment` varchar(45) NOT NULL,
  PRIMARY KEY (`IdAttachment`),
  KEY `fk_Attachment_Lesson1_idx` (`IdLesson`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `Card`
--

CREATE TABLE IF NOT EXISTS `Card` (
  `IdCard` int(11) NOT NULL AUTO_INCREMENT,
  `NumberCard` varchar(45) NOT NULL,
  `IdUser` int(11) NOT NULL,
  PRIMARY KEY (`IdCard`),
  KEY `fk_Card_User_idx` (`IdUser`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;

--
-- Contenu de la table `Card`
--

-- --------------------------------------------------------

--
-- Structure de la table `Category`
--

CREATE TABLE IF NOT EXISTS `Category` (
  `IdCategory` int(11) NOT NULL AUTO_INCREMENT,
  `NameCategory` varchar(45) NOT NULL,
  PRIMARY KEY (`IdCategory`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `Chapter`
--

CREATE TABLE IF NOT EXISTS `Chapter` (
  `IdChapter` int(11) NOT NULL AUTO_INCREMENT,
  `StatusChapter` enum('RECORDING','TRANSFER','WAITINGTREATMENT','TREATMENT','TREATED') NOT NULL,
  `DateChapter` datetime NOT NULL,
  `IdLesson` int(11) DEFAULT NULL,
  `IdUser` int(11) NOT NULL,
  `IdRecorder` int(11) NOT NULL,
  PRIMARY KEY (`IdChapter`),
  KEY `fk_Chapter_Recorder1_idx` (`IdRecorder`),
  KEY `fk_Chapter_RecorderUser1_idx` (`IdUser`),
  KEY `fk_Chapter_Lesson1_idx` (`IdLesson`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=19 ;


-- --------------------------------------------------------

--
-- Structure de la table `ConnectingModule`
--

CREATE TABLE IF NOT EXISTS `ConnectingModule` (
  `IdConnectingModule` int(11) NOT NULL AUTO_INCREMENT,
  `IdNetworkRecording` mediumint(9) NOT NULL,
  PRIMARY KEY (`IdConnectingModule`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;

--
-- Contenu de la table `ConnectingModule`
--


-- --------------------------------------------------------

--
-- Structure de la table `FileLesson`
--

CREATE TABLE IF NOT EXISTS `FileLesson` (
  `IdFileLesson` int(11) NOT NULL AUTO_INCREMENT,
  `IdChapter` int(11) NOT NULL,
  `TypeFile` enum('VIDEO_TRACKING','PHOTO_BOARD','VIDEO_BOARD','SOUND') NOT NULL,
  `NameFileLesson` varchar(45) NOT NULL,
  `StatutsFileLesson` enum('TRANSFER','WAITINGTREATMENT','TREATMENT','TREATED') NOT NULL,
  PRIMARY KEY (`IdFileLesson`),
  KEY `FK_FileLesson_IdLesson_idx` (`IdChapter`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=31 ;



-- --------------------------------------------------------

--
-- Structure de la table `Historic`
--

CREATE TABLE IF NOT EXISTS `Historic` (
  `DateTimeBegin` datetime NOT NULL,
  `IdUser` int(11) NOT NULL,
  `IdRecorder` int(11) NOT NULL,
  `DateTimeEnd` datetime NOT NULL,
  `Authorization` tinyint(1) NOT NULL,
  PRIMARY KEY (`DateTimeBegin`,`IdUser`,`IdRecorder`),
  KEY `FK_Historic_IdUser_idx` (`IdUser`),
  KEY `FK_Historic_IdRecorder` (`IdRecorder`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `Lesson`
--

CREATE TABLE IF NOT EXISTS `Lesson` (
  `IdLesson` int(11) NOT NULL AUTO_INCREMENT,
  `IdCategory` int(11) NOT NULL,
  `DateLesson` date NOT NULL,
  `StatusLesson` enum('RECORDING','TRANSFER','WAITINGTREATMENT','TREATMENT','TREATED','PUBLISHED','OFFLINE') NOT NULL,
  `NameLesson` text NOT NULL,
  PRIMARY KEY (`IdLesson`),
  KEY `fk_Lesson_Category1_idx` (`IdCategory`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `Recorder`
--

CREATE TABLE IF NOT EXISTS `Recorder` (
  `IdRecorder` int(11) NOT NULL AUTO_INCREMENT,
  `IdRoom` int(11) DEFAULT NULL,
  `IdConnectingModule` int(11) NOT NULL,
  `IdRecordingModule` int(11) NOT NULL,
  PRIMARY KEY (`IdRecorder`),
  KEY `fk_Recorder_RecordingModule1_idx` (`IdRecordingModule`),
  KEY `fk_Recorder_ConnectingModule1_idx` (`IdConnectingModule`),
  KEY `fk_Recorder_Room1_idx` (`IdRoom`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;

-- --------------------------------------------------------

--
-- Structure de la table `RecorderUser`
--

CREATE TABLE IF NOT EXISTS `RecorderUser` (
  `IdUser` int(11) NOT NULL,
  `DateBegin` datetime NOT NULL,
  `DateEnd` datetime NOT NULL,
  PRIMARY KEY (`IdUser`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- --------------------------------------------------------

--
-- Structure de la table `RecordingModule`
--

CREATE TABLE IF NOT EXISTS `RecordingModule` (
  `IdRecordingModule` int(11) NOT NULL AUTO_INCREMENT,
  `AdressMAC` varchar(12) NOT NULL,
  `RecordingConfiguration` blob,
  `IdNetwork` mediumint(9) NOT NULL,
  PRIMARY KEY (`IdRecordingModule`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;



-- --------------------------------------------------------

--
-- Structure de la table `Reference`
--

CREATE TABLE IF NOT EXISTS `Reference` (
  `TextReference` text NOT NULL,
  `IdChapter` int(11) NOT NULL,
  `TimeCode` datetime NOT NULL,
  `Relevance` int(11) NOT NULL,
  KEY `FK_Reference_IdLesson_idx` (`IdChapter`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `Room`
--

CREATE TABLE IF NOT EXISTS `Room` (
  `IdRoom` int(11) NOT NULL AUTO_INCREMENT,
  `RoomName` varchar(45) NOT NULL,
  `RoomDescription` text,
  PRIMARY KEY (`IdRoom`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;


-- --------------------------------------------------------

--
-- Structure de la table `User`
--

CREATE TABLE IF NOT EXISTS `User` (
  `IdUser` int(11) NOT NULL AUTO_INCREMENT,
  `FirstName` varchar(45) NOT NULL,
  `LastName` varchar(45) NOT NULL,
  `Password` varchar(45) NOT NULL,
  `Email` varchar(45) NOT NULL,
  PRIMARY KEY (`IdUser`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;


-- --------------------------------------------------------

--
-- Structure de la table `WebsiteUser`
--

CREATE TABLE IF NOT EXISTS `WebsiteUser` (
  `IdUser` int(11) NOT NULL,
  `DateRegistration` datetime NOT NULL,
  PRIMARY KEY (`IdUser`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour les tables exportées
--

--
-- Contraintes pour la table `Attachment`
--
ALTER TABLE `Attachment`
  ADD CONSTRAINT `FK_Attachment_IdLesson` FOREIGN KEY (`IdLesson`) REFERENCES `Lesson` (`IdLesson`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Card`
--
ALTER TABLE `Card`
  ADD CONSTRAINT `FK_Card_IdUser` FOREIGN KEY (`IdUser`) REFERENCES `RecorderUser` (`IdUser`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Chapter`
--
ALTER TABLE `Chapter`
  ADD CONSTRAINT `fk_Chapter_IdLesson` FOREIGN KEY (`IdLesson`) REFERENCES `Lesson` (`IdLesson`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `fk_Chapter_IdRecorder` FOREIGN KEY (`IdRecorder`) REFERENCES `Recorder` (`IdRecorder`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `fk_Chapter_IdUser` FOREIGN KEY (`IdUser`) REFERENCES `RecorderUser` (`IdUser`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `FileLesson`
--
ALTER TABLE `FileLesson`
  ADD CONSTRAINT `FK_FileLesson_IdChapter` FOREIGN KEY (`IdChapter`) REFERENCES `Chapter` (`IdChapter`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Historic`
--
ALTER TABLE `Historic`
  ADD CONSTRAINT `FK_Historic_IdRecorder` FOREIGN KEY (`IdRecorder`) REFERENCES `Recorder` (`IdRecorder`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `FK_Historic_IdUser` FOREIGN KEY (`IdUser`) REFERENCES `RecorderUser` (`IdUser`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Lesson`
--
ALTER TABLE `Lesson`
  ADD CONSTRAINT `FK_Lesson_IdCategory` FOREIGN KEY (`IdCategory`) REFERENCES `Category` (`IdCategory`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Recorder`
--
ALTER TABLE `Recorder`
  ADD CONSTRAINT `FK_Recorder_IdConnectingModule` FOREIGN KEY (`IdConnectingModule`) REFERENCES `ConnectingModule` (`IdConnectingModule`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `FK_Recorder_IdRecordingModule` FOREIGN KEY (`IdRecordingModule`) REFERENCES `RecordingModule` (`IdRecordingModule`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `FK_Recorder_IdRoom` FOREIGN KEY (`IdRoom`) REFERENCES `Room` (`IdRoom`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `RecorderUser`
--
ALTER TABLE `RecorderUser`
  ADD CONSTRAINT `FK_UserRecorder_IdUser` FOREIGN KEY (`IdUser`) REFERENCES `User` (`IdUser`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `Reference`
--
ALTER TABLE `Reference`
  ADD CONSTRAINT `FK_Reference_IdChapter` FOREIGN KEY (`IdChapter`) REFERENCES `Chapter` (`IdChapter`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Contraintes pour la table `WebsiteUser`
--
ALTER TABLE `WebsiteUser`
  ADD CONSTRAINT `FK_UserWebsite_IdUser` FOREIGN KEY (`IdUser`) REFERENCES `User` (`IdUser`) ON DELETE NO ACTION ON UPDATE NO ACTION;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
